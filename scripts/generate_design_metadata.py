"""Regenerate the native design sidecar from DESIGN.md and UI layout source."""
from pathlib import Path
import json
import re
root = Path(__file__).resolve().parent.parent
source = (root / 'DESIGN.md').read_text()
colors = dict(re.findall(r'^  ([a-z-]+): "(#[0-9a-f]{6})"$', source, re.M))
assert len(colors) == 10, 'Expected complete dark palette in DESIGN.md'
metadata = {
    'schemaVersion': 2,
    'title': 'Design System: CWaypoint',
    'extensions': {'shadows': [], 'motion': [], 'breakpoints': [1100],
        'native': {'source': 'src/main.cpp', 'renderer': 'raylib',
            'tokens': colors, 'navigation': 'top bar with logo, Courses and gear menu',
            'courseWorkspace': 'left module list, central material, right offline study assistant',
            'window': {'initial': [1280, 840], 'minimum': [960, 640]},
            'contentWidth': 'min(1200, windowWidth - 64)',
            'accessibility': 'Custom controls are not exposed to VoiceOver.',
            'previewCaveat': 'Use native captures under .scratch/dark-ui; HTML previews are not runtime evidence.'}},
    'components': [],
    'narrative': {'northStar': 'The Dark Study Workspace',
        'overview': 'Charcoal course catalogue with module navigation, central material and an offline study assistant.',
        'keyCharacteristics': ['Both C and C++ courses in one catalogue.',
            'Three-panel course workspace at standard and compact widths.',
            'Explicit feedback and preserved learning evidence.']}}
(root / '.impeccable/design.json').write_text(json.dumps(metadata, indent=2) + '\n')
