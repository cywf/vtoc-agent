//! Hardware-neutral OLED content for the first VTOC Agent profile.
//!
//! Board bring-up owns the eventual display driver and pin mapping. This module
//! owns only the truthful, bounded state that the driver may render.

/// The currently active field workflow shown on the device.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DisplayMode {
    Standalone,
    Sentinel,
    Ragnar,
    Intercept,
    Tak,
}

/// Connection state shown without exposing credentials or network identifiers.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum LinkState {
    Offline,
    Connecting,
    Connected,
}

/// A minimal display frame that can fit an OLED status card.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DisplayFrame {
    pub title: &'static str,
    pub primary: String,
    pub secondary: String,
    pub link: LinkState,
}

/// Produces a mode-specific status card from verified local state.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct DisplayState {
    pub mode: DisplayMode,
    pub link: LinkState,
    pub queued_jobs: u8,
    pub result_is_fresh: bool,
}

impl DisplayState {
    pub fn frame(self) -> DisplayFrame {
        let (title, primary) = match self.mode {
            DisplayMode::Standalone => ("VTOC Agent", "Local control ready"),
            DisplayMode::Sentinel => ("Sentinel", "Local observations"),
            DisplayMode::Ragnar => ("Ragnar", "Remote job status"),
            DisplayMode::Intercept => ("Intercept", "Remote job status"),
            DisplayMode::Tak => ("TAK", "Shared map status"),
        };
        let secondary = if self.queued_jobs > 0 {
            format!("{} job(s) queued", self.queued_jobs)
        } else if self.result_is_fresh {
            "Latest result current".into()
        } else {
            "No current result".into()
        };

        DisplayFrame {
            title,
            primary: primary.into(),
            secondary,
            link: self.link,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn each_supported_mode_has_a_distinct_title() {
        let modes = [
            (DisplayMode::Standalone, "VTOC Agent"),
            (DisplayMode::Sentinel, "Sentinel"),
            (DisplayMode::Ragnar, "Ragnar"),
            (DisplayMode::Intercept, "Intercept"),
            (DisplayMode::Tak, "TAK"),
        ];

        for (mode, title) in modes {
            assert_eq!(
                DisplayState {
                    mode,
                    link: LinkState::Offline,
                    queued_jobs: 0,
                    result_is_fresh: false,
                }
                .frame()
                .title,
                title
            );
        }
    }

    #[test]
    fn queued_work_takes_priority_over_stale_or_fresh_result_text() {
        let frame = DisplayState {
            mode: DisplayMode::Ragnar,
            link: LinkState::Connected,
            queued_jobs: 2,
            result_is_fresh: true,
        }
        .frame();

        assert_eq!(frame.primary, "Remote job status");
        assert_eq!(frame.secondary, "2 job(s) queued");
        assert_eq!(frame.link, LinkState::Connected);
    }

    #[test]
    fn result_freshness_is_explicit_when_no_work_is_queued() {
        let fresh = DisplayState {
            mode: DisplayMode::Tak,
            link: LinkState::Connected,
            queued_jobs: 0,
            result_is_fresh: true,
        }
        .frame();
        let stale = DisplayState {
            result_is_fresh: false,
            ..DisplayState {
                mode: DisplayMode::Tak,
                link: LinkState::Connected,
                queued_jobs: 0,
                result_is_fresh: true,
            }
        }
        .frame();

        assert_eq!(fresh.secondary, "Latest result current");
        assert_eq!(stale.secondary, "No current result");
    }
}
