//! Deterministic, offline rules for declared board-native capabilities.
//!
//! Rules are deliberately narrow: a known local event may request one known
//! capability.  They cannot execute arbitrary commands, use an arbitrary
//! adapter, or bypass the capability catalog.

use crate::{Capability, CapabilityCatalog, CapabilityRequest, ResultState};

/// Local events the first firmware profile may observe without remote input.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum LocalEvent {
    NetworkConnected,
    ScheduledHealthCheck,
    UserRequestedObservation,
}

/// A saved local rule. The user-facing layer must create this from validated
/// configuration; this module only evaluates it deterministically.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct LocalRule {
    pub id: String,
    pub event: LocalEvent,
    pub capability: Capability,
    pub enabled: bool,
}

/// The decision made for a single event/rule pair.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum RuleDecision {
    NotTriggered,
    Disabled,
    Queued(CapabilityRequest),
    Unavailable {
        capability: Capability,
        summary: String,
    },
}

/// Evaluates only saved, typed rules against a board capability catalog.
#[derive(Debug, Clone)]
pub struct LocalRulesEngine {
    catalog: CapabilityCatalog,
}

impl LocalRulesEngine {
    pub fn new(catalog: CapabilityCatalog) -> Self {
        Self { catalog }
    }

    /// Produces a request only when the event matches an enabled rule and the
    /// requested capability is available on the declared board profile.
    pub fn evaluate(
        &self,
        event: LocalEvent,
        rule: &LocalRule,
        request_id: impl Into<String>,
    ) -> RuleDecision {
        if rule.event != event {
            return RuleDecision::NotTriggered;
        }
        if !rule.enabled {
            return RuleDecision::Disabled;
        }
        if !self.catalog.supports(rule.capability) {
            return RuleDecision::Unavailable {
                capability: rule.capability,
                summary: "declared capability is unavailable on this board profile".into(),
            };
        }
        RuleDecision::Queued(CapabilityRequest {
            request_id: request_id.into(),
            capability: rule.capability,
        })
    }

    /// Converts an unavailable decision into the same result state exposed by
    /// the capability protocol, keeping local and remote reporting aligned.
    pub fn state_for(decision: &RuleDecision) -> Option<ResultState> {
        match decision {
            RuleDecision::Unavailable { .. } => Some(ResultState::Unavailable),
            RuleDecision::Queued(_) => Some(ResultState::Queued),
            RuleDecision::NotTriggered | RuleDecision::Disabled => None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn rule(event: LocalEvent, capability: Capability) -> LocalRule {
        LocalRule {
            id: "health-on-link".into(),
            event,
            capability,
            enabled: true,
        }
    }

    #[test]
    fn matching_enabled_rule_queues_a_declared_capability() {
        let engine = LocalRulesEngine::new(CapabilityCatalog::heltec_v3(false));
        let decision = engine.evaluate(
            LocalEvent::NetworkConnected,
            &rule(LocalEvent::NetworkConnected, Capability::DeviceHealth),
            "rule-1",
        );

        assert_eq!(
            decision,
            RuleDecision::Queued(CapabilityRequest {
                request_id: "rule-1".into(),
                capability: Capability::DeviceHealth,
            })
        );
    }

    #[test]
    fn unavailable_adapter_never_queues_from_a_rule() {
        let engine = LocalRulesEngine::new(CapabilityCatalog::heltec_v3(false));
        let decision = engine.evaluate(
            LocalEvent::NetworkConnected,
            &rule(
                LocalEvent::NetworkConnected,
                Capability::ExternalAdapterHealth,
            ),
            "rule-2",
        );

        assert!(matches!(
            decision,
            RuleDecision::Unavailable {
                capability: Capability::ExternalAdapterHealth,
                ..
            }
        ));
        assert_eq!(
            LocalRulesEngine::state_for(&decision),
            Some(ResultState::Unavailable)
        );
    }

    #[test]
    fn disabled_and_non_matching_rules_do_not_run() {
        let engine = LocalRulesEngine::new(CapabilityCatalog::heltec_v3(false));
        let mut disabled = rule(LocalEvent::NetworkConnected, Capability::DeviceHealth);
        disabled.enabled = false;

        assert_eq!(
            engine.evaluate(LocalEvent::NetworkConnected, &disabled, "rule-3"),
            RuleDecision::Disabled
        );
        assert_eq!(
            engine.evaluate(
                LocalEvent::ScheduledHealthCheck,
                &rule(LocalEvent::NetworkConnected, Capability::DeviceHealth),
                "rule-4",
            ),
            RuleDecision::NotTriggered
        );
    }
}
