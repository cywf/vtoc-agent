const release = document.querySelector('#release');
const browserStatus = document.querySelector('#browser-status');
const selectPort = document.querySelector('#select-port');

function renderManifest(manifest) {
  const fields = [
    ['Status', manifest.status],
    ['Version', manifest.version ?? 'Not published'],
    ['Supported board', manifest.supportedBoard],
    ['Checksum', manifest.sha256 ?? 'Not published'],
    ['Recovery image', manifest.recoveryImage ?? 'Not published'],
  ];
  release.replaceChildren(...fields.flatMap(([key, value]) => {
    const term = document.createElement('dt');
    const description = document.createElement('dd');
    term.textContent = key;
    description.textContent = value;
    return [term, description];
  }));
}

async function loadManifest() {
  const response = await fetch('./releases/manifest.json', { cache: 'no-store' });
  if (!response.ok) throw new Error('Release manifest is unavailable.');
  const manifest = await response.json();
  const noRelease = manifest.status === 'no-release'
    && manifest.version === null
    && manifest.sha256 === null
    && manifest.recoveryImage === null;
  const reviewableRelease = manifest.status === 'ready-for-review'
    && typeof manifest.version === 'string'
    && typeof manifest.firmwareImage === 'string'
    && /^[a-f0-9]{64}$/i.test(manifest.sha256)
    && typeof manifest.recoveryImage === 'string'
    && typeof manifest.recoveryEvidence === 'string';
  if (!noRelease && !reviewableRelease) {
    throw new Error('Release manifest is incomplete or unsupported. Flashing remains disabled.');
  }
  renderManifest(manifest);
  return manifest;
}

function configureWebSerial() {
  if (!('serial' in navigator)) {
    browserStatus.textContent = 'Web Serial is unavailable. Use a current Chromium-based browser over HTTPS.';
    return;
  }
  browserStatus.textContent = 'Web Serial is available. No device is selected.';
  selectPort.disabled = false;
  selectPort.addEventListener('click', async () => {
    try {
      await navigator.serial.requestPort();
      browserStatus.textContent = 'A serial port was selected. Flashing is still disabled until a verified release is published.';
    } catch {
      browserStatus.textContent = 'No serial port was selected.';
    }
  });
}

loadManifest().catch((error) => { release.textContent = error.message; });
configureWebSerial();
