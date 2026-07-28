//! Bounded, board-aware Huginn core.

pub mod board;
pub mod display;
pub mod rules;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Capability {
    DeviceHealth,
    WifiObservation,
    BleObservation,
    MeshtasticStatus,
    ExternalAdapterHealth,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ResultState {
    Queued,
    Completed,
    Denied,
    Unsupported,
    Unavailable,
    TimedOut,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CapabilityRequest {
    pub request_id: String,
    pub capability: Capability,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CapabilityResult {
    pub request_id: String,
    pub state: ResultState,
    pub summary: String,
}

pub trait CapabilityExecutor {
    fn execute(&self, request: &CapabilityRequest) -> CapabilityResult;
}

#[derive(Debug, Clone)]
pub struct CapabilityCatalog {
    external_adapter_present: bool,
}

impl CapabilityCatalog {
    pub fn heltec_v3(external_adapter_present: bool) -> Self {
        Self {
            external_adapter_present,
        }
    }
    pub fn supports(&self, capability: Capability) -> bool {
        !matches!(capability, Capability::ExternalAdapterHealth) || self.external_adapter_present
    }
    fn unavailable(&self, request: &CapabilityRequest) -> CapabilityResult {
        CapabilityResult {
            request_id: request.request_id.clone(),
            state: ResultState::Unavailable,
            summary: "declared external adapter is unavailable".into(),
        }
    }
}

pub struct GuardedExecutor<E> {
    catalog: CapabilityCatalog,
    inner: E,
}
impl<E> GuardedExecutor<E> {
    pub fn new(catalog: CapabilityCatalog, inner: E) -> Self {
        Self { catalog, inner }
    }
}
impl<E: CapabilityExecutor> CapabilityExecutor for GuardedExecutor<E> {
    fn execute(&self, request: &CapabilityRequest) -> CapabilityResult {
        if self.catalog.supports(request.capability) {
            self.inner.execute(request)
        } else {
            self.catalog.unavailable(request)
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    struct Fixture;
    impl CapabilityExecutor for Fixture {
        fn execute(&self, request: &CapabilityRequest) -> CapabilityResult {
            CapabilityResult {
                request_id: request.request_id.clone(),
                state: ResultState::Completed,
                summary: "verified health fixture".into(),
            }
        }
    }
    #[test]
    fn absent_adapter_is_unavailable() {
        let guarded = GuardedExecutor::new(CapabilityCatalog::heltec_v3(false), Fixture);
        let result = guarded.execute(&CapabilityRequest {
            request_id: "fixture".into(),
            capability: Capability::ExternalAdapterHealth,
        });
        assert_eq!(result.state, ResultState::Unavailable);
    }
}
