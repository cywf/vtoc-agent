//! Hardware-neutral contracts for the only supported 0.x board profile.
//!
//! This module deliberately contains no pin assignments, drivers, flashing
//! code, OTA support, or hardware detection. Those details require evidence
//! from the exact physical board before they can be implemented.

/// The sole board identity accepted by the 0.x firmware foundation.
pub const HELTEC_WIFI_LORA_32_V3: &str = "heltec-wifi-lora-32-v3";

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct BoardProfile {
    pub id: &'static str,
    pub display_name: &'static str,
    pub hardware_evidence_required: bool,
    pub pin_mapping_available: bool,
    pub ota_supported: bool,
}

impl BoardProfile {
    /// Returns a conservative profile with no inferred electrical details.
    pub const fn heltec_wifi_lora_32_v3() -> Self {
        Self {
            id: HELTEC_WIFI_LORA_32_V3,
            display_name: "Heltec WiFi LoRa 32 V3",
            hardware_evidence_required: true,
            pin_mapping_available: false,
            ota_supported: false,
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct SerialRecoveryEvidence {
    pub artifact_version_recorded: bool,
    pub checksum_recorded: bool,
    pub serial_boot_log_captured: bool,
    pub memory_report_captured: bool,
    pub invalid_configuration_recovery_passed: bool,
    pub rollback_image_verified: bool,
}

impl SerialRecoveryEvidence {
    /// This only establishes that an artifact has the required review evidence.
    /// It is not authorization to flash a device.
    pub const fn is_complete(self) -> bool {
        self.artifact_version_recorded
            && self.checksum_recorded
            && self.serial_boot_log_captured
            && self.memory_report_captured
            && self.invalid_configuration_recovery_passed
            && self.rollback_image_verified
    }
}

/// The recovery policy is serial-only until physical-board evidence is reviewed.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct SerialRecoveryContract {
    pub serial_only: bool,
    pub automatic_flashing_allowed: bool,
    pub ota_allowed: bool,
}

impl SerialRecoveryContract {
    pub const fn for_unverified_board() -> Self {
        Self {
            serial_only: true,
            automatic_flashing_allowed: false,
            ota_allowed: false,
        }
    }

    pub const fn evidence_is_ready_for_review(self, evidence: SerialRecoveryEvidence) -> bool {
        self.serial_only
            && !self.automatic_flashing_allowed
            && !self.ota_allowed
            && evidence.is_complete()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn heltec_profile_does_not_infer_hardware_details() {
        let profile = BoardProfile::heltec_wifi_lora_32_v3();
        assert_eq!(profile.id, HELTEC_WIFI_LORA_32_V3);
        assert!(profile.hardware_evidence_required);
        assert!(!profile.pin_mapping_available);
        assert!(!profile.ota_supported);
    }

    #[test]
    fn incomplete_evidence_cannot_pass_recovery_review() {
        let contract = SerialRecoveryContract::for_unverified_board();
        let evidence = SerialRecoveryEvidence {
            artifact_version_recorded: true,
            checksum_recorded: true,
            serial_boot_log_captured: false,
            memory_report_captured: false,
            invalid_configuration_recovery_passed: false,
            rollback_image_verified: false,
        };
        assert!(!contract.evidence_is_ready_for_review(evidence));
    }

    #[test]
    fn complete_evidence_is_reviewable_but_not_a_flash_authorization() {
        let contract = SerialRecoveryContract::for_unverified_board();
        let evidence = SerialRecoveryEvidence {
            artifact_version_recorded: true,
            checksum_recorded: true,
            serial_boot_log_captured: true,
            memory_report_captured: true,
            invalid_configuration_recovery_passed: true,
            rollback_image_verified: true,
        };
        assert!(contract.evidence_is_ready_for_review(evidence));
        assert!(!contract.automatic_flashing_allowed);
        assert!(!contract.ota_allowed);
    }
}
