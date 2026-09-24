"""Load, render, and check the character description ladder.

Source of truth: ai_agent_docs/character_levels/src/characters.json
Rendered docs are committed so an agent can read them without running this.
`render --check` fails if the committed docs drift from the source.
"""

from __future__ import annotations

import json
from pathlib import Path

from sprite_pipeline.prompts import sprite_prompt

ROOT = Path(__file__).resolve().parents[2]
SOURCE_DIR = ROOT / "ai_agent_docs" / "character_levels" / "src"
LEVEL_DIR = ROOT / "ai_agent_docs" / "character_levels"
CAST_JSON = ROOT / "ai_agent_docs" / "skills" / "chrono-sprite" / "assets" / "cast.json"
CHAR_DIR = ROOT / "characters"

# Budgets are wide enough for real prose and tight enough that a level
# cannot be a copy of the one below it. L3 is a production note, not a novel.
LEVEL_BUDGETS = {
    "L0": (22, 55),
    "L1": (70, 160),
    "L2": (180, 420),
    "L3": (320, 900),
}
L4_CHARS = (700, 1500)

# Words that must not appear in description docs or prompts. The legal
# rule document may name these; the art docs must not.
DENY = (
    "bikini",
    "nude",
    "naked",
    "lingerie",
    "upskirt",
    "cleavage",
    "erotic",
    "nsfw",
    "midriff",
    "swimwear",
)

REQUIRED_EXPRESSIONS = ("neutral", "happy", "sad", "surprised", "blush")
ZONE_KEYS = (
    "hair", "skin", "eye", "tops1", "tops2", "waist",
    "bottom1", "bottom2", "shoes", "decoration1", "decoration2",
)


def words(text: str) -> int:
    return len(text.split())


def load(path: Path | None = None) -> dict:
    """Load one combined document.

    The committed source is a directory: `_shared.json` plus one `<id>.json`
    per character. Tests may pass a single combined JSON file instead.
    """
    if path is not None:
        doc = json.loads(path.read_text(encoding="utf-8"))
        if not doc.get("characters"):
            raise SystemExit(f"{path} has no characters")
        return doc
    shared_path = SOURCE_DIR / "_shared.json"
    if not shared_path.exists():
        raise SystemExit(f"missing {shared_path}")
    doc = json.loads(shared_path.read_text(encoding="utf-8"))
    characters = []
    for path in sorted(SOURCE_DIR.glob("*.json")):
        if path.name.startswith("_"):
            continue
        characters.append(json.loads(path.read_text(encoding="utf-8")))
    if not characters:
        raise SystemExit(f"no character JSON in {SOURCE_DIR}")
    doc["characters"] = characters
    return doc


def hex_rgb(value: str) -> tuple[int, int, int]:
    raw = value.strip().lstrip("#")
    if len(raw) != 6:
        raise ValueError(f"bad hex {value!r}")
    return tuple(int(raw[i:i + 2], 16) for i in (0, 2, 4))  # type: ignore[return-value]


def hex_distance(a: str, b: str) -> float:
    ra, rb = hex_rgb(a), hex_rgb(b)
    return sum((x - y) ** 2 for x, y in zip(ra, rb)) ** 0.5


def l4_of(doc: dict, ch: dict) -> str:
    return sprite_prompt(doc, ch)


def render_character(doc: dict, ch: dict) -> str:
    l4 = l4_of(doc, ch)
    levels = ch["levels"]
    counts = {key: words(levels[key]) for key in ("L0", "L1", "L2", "L3")}
    lore = "\n".join(f"- `{path}`" for path in ch["lore"])
    questions = "\n".join(f"- {q}" for q in doc["shared_open_questions"] + ch.get("open_questions", []))
    aliases = []
    for expr, spec in ch["expressions"].items():
        names = ", ".join(spec.get("alias") or []) or "—"
        aliases.append(f"| {expr} | {names} | {spec['read']} |")
    zones = "\n".join(
        f"- **{key}**: {ch['palette']['zones'][key]['name']} `{ch['palette']['zones'][key]['hex']}`"
        for key in ZONE_KEYS
    )
    banned = "\n".join(f"- {item}" for item in ch["banned"])
    return f"""# {ch['name']} — description levels

`id: {ch['id']}` · appeal track `{ch['appeal_track']}` · approved: `{str(ch['approved']).lower()}`

{ch['age_status']}

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | {counts['L0']} words | roster, thumbnail, silhouette test | never |
| L1 Card | {counts['L1']} words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | {counts['L2']} words | design review, first composition without construction | never |
| L3 Production | {counts['L3']} words | QA, garment construction, expression geometry | never |
| L4 Prompt | {len(l4)} chars | you are about to generate the default sprite | yes, saved to `characters/{ch['id']}/prompts/` first |

## Lore

{lore}

## L0 Glance

{levels['L0']}

## L1 Card

{levels['L1']}

## L2 Design

{levels['L2']}

## L3 Production

{levels['L3']}

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
{l4}
```

## Canon card

- **Memory point:** {ch['memory_point']}
- **Secondary hook:** {ch['secondary_hook']}
- **Silhouette class:** {ch['silhouette']['class']}
- **Masses:** big — {ch['silhouette']['big']}; mid — {ch['silhouette']['mid']}; small — {ch['silhouette']['small']}
- **Negative space:** {ch['silhouette']['negative_space']}
- **Main colour:** {ch['palette']['main']['name']} `{ch['palette']['main']['hex']}` — {ch['palette']['main']['means']}
- **Default outfit:** {ch['wardrobe']['default']['name']}
- **Alternate outfit:** {ch['wardrobe']['alternate']['name']}
- **Shape majority:** {ch['shape_majority']} ({ch['shape']['circle']} circle / {ch['shape']['square']} square / {ch['shape']['triangle']} triangle)
- **Legacy:** {ch['legacy']}

### Colour zones

{zones}

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
{chr(10).join(aliases)}

### Banned in every prompt

{banned}

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

{questions}
"""


def render_glance(doc: dict) -> str:
    rows = []
    blocks = []
    for ch in doc["characters"]:
        main = ch["palette"]["main"]
        rows.append(
            f"| `{ch['id']}` | {ch['silhouette']['class']} | "
            f"{main['name']} `{main['hex']}` | {ch['shape_majority']} | {ch['memory_point']} |"
        )
        blocks.append(f"### {ch['name']}\n\n{ch['levels']['L0']}\n")
    table = "\n".join(rows)
    body = "\n".join(blocks)
    return f"""# Cast at a glance

This is every speaking character at **L0** — the detail level used for roster differentiation and the thumbnail test. If two rows could be the same person with the colour swapped, the designs have not separated yet.

Any two characters must differ in silhouette class and in main-colour distance. Shape majority is a third lever, not a substitute. Full ladders: `ai_agent_docs/character_levels/<id>.md`.

| id | silhouette class | main colour | shape | memory point |
|---|---|---|---|---|
{table}

## L0 texts

{body}
"""


def render_cast(doc: dict) -> dict:
    characters = []
    for ch in doc["characters"]:
        characters.append({
            "id": ch["id"],
            "name": ch["name"],
            "name_local": ch["name_local"],
            "role": ch["role"],
            "approved": ch["approved"],
            "appeal_track": ch["appeal_track"],
            "age_status": ch["age_status"],
            "memory_point": ch["memory_point"],
            "silhouette_class": ch["silhouette"]["class"],
            "main_colour": ch["palette"]["main"],
            "shape_majority": ch["shape_majority"],
            "default_outfit": ch["wardrobe"]["default"]["name"],
            "alternate_outfit": ch["wardrobe"]["alternate"]["name"],
            "banned": ch["banned"],
            "level_doc": f"ai_agent_docs/character_levels/{ch['id']}.md",
            "source": f"ai_agent_docs/character_levels/src/{ch['id']}.json",
        })
    return {
        "schema_version": "1.0.0",
        "status": "DRAFT — approved is false on every character until the project owner signs the level doc.",
        "generated_from": "ai_agent_docs/character_levels/src/",
        "do_not_edit": "Regenerate with tools/render_character_levels.py.",
        "style_bible": doc["style_bible"],
        "required_expressions": doc["required_expressions"],
        "prompt_char_max": doc["prompt_char_max"],
        "characters": characters,
    }


def prompt_sidecar(ch: dict, l4: str) -> str:
    return (
        f"# 00_level_sprite — {ch['id']}\n\n"
        "Verdict: UNSENT\n\n"
        "This file is the current L4 sprite prompt, rendered from\n"
        "`ai_agent_docs/character_levels/src/`.\n"
        "It has not been sent to a generator.\n\n"
        "When you send a prompt, copy the text you actually send to the next\n"
        "numbered file (`01_sprite.txt`, `02_sprite.txt`, …) and write a\n"
        "result note beside it. Do not edit this rendered file by hand.\n"
        "Change the level source and re-render.\n\n"
        "```text\n"
        f"{l4}\n"
        "```\n"
    )


def prompt_text(l4: str) -> str:
    return l4 + "\n"


def outputs(doc: dict) -> dict[Path, str]:
    """Path -> text for every generated file."""
    files: dict[Path, str] = {}
    files[LEVEL_DIR / "CAST_AT_A_GLANCE.md"] = render_glance(doc)
    files[CAST_JSON] = json.dumps(render_cast(doc), indent=2, ensure_ascii=False) + "\n"
    for ch in doc["characters"]:
        l4 = l4_of(doc, ch)
        files[LEVEL_DIR / f"{ch['id']}.md"] = render_character(doc, ch)
        prompt_dir = CHAR_DIR / ch["id"] / "prompts"
        files[prompt_dir / "00_level_sprite.txt"] = prompt_text(l4)
        files[prompt_dir / "00_level_sprite.result.md"] = prompt_sidecar(ch, l4)
    return files


def write_outputs(doc: dict) -> list[Path]:
    written = []
    for path, text in outputs(doc).items():
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        written.append(path)
    return written


def diff_outputs(doc: dict) -> list[str]:
    errors = []
    for path, text in outputs(doc).items():
        if not path.exists():
            errors.append(f"missing rendered file {path.relative_to(ROOT)}")
            continue
        current = path.read_text(encoding="utf-8")
        if current != text:
            errors.append(f"stale rendered file {path.relative_to(ROOT)} — re-run tools/render_character_levels.py")
    return errors


def check(doc: dict, include_diff: bool = True) -> list[str]:
    errors: list[str] = []
    errors.extend(_check_doc(doc))
    seen_id = set()
    seen_class = set()
    seen_memory = set()
    mains: list[tuple[str, str]] = []
    for ch in doc["characters"]:
        errors.extend(_check_character(doc, ch))
        if ch["id"] in seen_id:
            errors.append(f"duplicate id {ch['id']}")
        seen_id.add(ch["id"])
        if ch["silhouette"]["class"] in seen_class:
            errors.append(f"silhouette class not unique: {ch['silhouette']['class']}")
        seen_class.add(ch["silhouette"]["class"])
        if ch["memory_point"] in seen_memory:
            errors.append(f"memory point not unique: {ch['memory_point']}")
        seen_memory.add(ch["memory_point"])
        mains.append((ch["id"], ch["palette"]["main"]["hex"]))
    for i, (ida, hexa) in enumerate(mains):
        for idb, hexb in mains[i + 1:]:
            distance = hex_distance(hexa, hexb)
            if distance < 80:
                errors.append(
                    f"main colours too close: {ida} {hexa} vs {idb} {hexb} "
                    f"(distance {distance:.1f}, need >= 80)"
                )
    if include_diff:
        errors.extend(diff_outputs(doc))
    return errors


def _check_doc(doc: dict) -> list[str]:
    errors = []
    for key in ("style_bible", "global_exclude", "required_expressions", "shared_open_questions"):
        if key not in doc:
            errors.append(f"source missing {key}")
    if tuple(doc.get("required_expressions", [])) != REQUIRED_EXPRESSIONS:
        errors.append(f"required_expressions must be {REQUIRED_EXPRESSIONS}")
    if int(doc.get("prompt_char_max", 0)) != L4_CHARS[1]:
        errors.append(f"prompt_char_max must be {L4_CHARS[1]}")
    return errors


def _deny_scan(label: str, text: str) -> list[str]:
    lowered = text.lower()
    return [f"{label} contains denied word {word!r}" for word in DENY if word in lowered]


def _check_character(doc: dict, ch: dict) -> list[str]:
    errors = []
    cid = ch["id"]
    if ch["approved"] is not False:
        errors.append(f"{cid}: approved must stay false until the owner signs it")
    if ch["appeal_track"] not in ("presence", "cool", "shadow"):
        errors.append(f"{cid}: bad appeal_track {ch['appeal_track']!r}")
    shape = ch["shape"]
    total = shape["circle"] + shape["square"] + shape["triangle"]
    if abs(total - 1.0) > 0.02:
        errors.append(f"{cid}: shape ratios sum to {total}, not 1")
    majority = max(shape, key=shape.get)
    if majority != ch["shape_majority"]:
        errors.append(f"{cid}: shape_majority is {ch['shape_majority']}, ratios say {majority}")
    zones = ch["palette"]["zones"]
    missing = [key for key in ZONE_KEYS if key not in zones]
    if missing:
        errors.append(f"{cid}: missing colour zones {missing}")
    for expr in REQUIRED_EXPRESSIONS:
        if expr not in ch["expressions"]:
            errors.append(f"{cid}: missing expression {expr}")
        else:
            spec = ch["expressions"][expr]
            for field in ("read", "brow", "eye", "mouth"):
                if not spec.get(field):
                    errors.append(f"{cid}: expression {expr} missing {field}")
    for path in ch["lore"]:
        if not (ROOT / path).exists():
            errors.append(f"{cid}: lore path missing {path}")
    if not ch["banned"]:
        errors.append(f"{cid}: banned list is empty")
    memory = ch["memory_point"]
    if memory.lower() not in ch["silhouette"]["glance"].lower() and memory not in ch["levels"]["L0"]:
        pass
    levels = ch["levels"]
    counts = []
    for key, (low, high) in LEVEL_BUDGETS.items():
        count = words(levels[key])
        counts.append(count)
        if not (low <= count <= high):
            errors.append(f"{cid}: {key} is {count} words, budget {low}-{high}")
        if memory not in levels[key]:
            errors.append(f"{cid}: {key} does not contain the memory point verbatim")
    if counts != sorted(counts) or len(set(counts)) != len(counts):
        errors.append(f"{cid}: level word counts must strictly increase, got {counts}")
    if ch["silhouette"]["class"] not in levels["L0"]:
        errors.append(f"{cid}: L0 must contain the silhouette class verbatim")
    if "#" in levels["L0"] or "#" in levels["L1"]:
        errors.append(f"{cid}: L0 and L1 must not carry hex codes")
    main_hex = ch["palette"]["main"]["hex"]
    if main_hex not in levels["L2"] or main_hex not in levels["L3"]:
        errors.append(f"{cid}: L2 and L3 must contain the main hex {main_hex}")
    outfit = ch["wardrobe"]["default"]["name"]
    if outfit not in levels["L1"] or outfit not in levels["L2"] or outfit not in levels["L3"]:
        errors.append(f"{cid}: L1-L3 must name the default outfit {outfit!r}")
    design = ch["wardrobe"]["default"]["design"]
    production = ch["wardrobe"]["default"]["production"]
    if design not in levels["L2"] or design not in levels["L3"]:
        errors.append(f"{cid}: L2 and L3 must contain the default design sentence")
    if production not in levels["L3"]:
        errors.append(f"{cid}: L3 must contain the default production paragraph")
    if production in levels["L0"] or production in levels["L1"] or production in levels["L2"]:
        errors.append(f"{cid}: production paragraph leaked into a lower level")
    if design in levels["L0"] or design in levels["L1"]:
        errors.append(f"{cid}: design sentence leaked into L0 or L1")
    if ch["legacy"] not in levels["L3"]:
        errors.append(f"{cid}: L3 must contain the legacy note verbatim")
    for key in ("L0", "L1", "L2"):
        errors.extend(_deny_scan(f"{cid} {key}", levels[key]))
    errors.extend(_deny_scan(f"{cid} L3", levels["L3"]))
    errors.extend(_deny_scan(f"{cid} legacy", ch["legacy"]))
    errors.extend(_deny_scan(f"{cid} banned", " ".join(ch["banned"])))

    l4 = l4_of(doc, ch)
    if not (L4_CHARS[0] <= len(l4) <= L4_CHARS[1]):
        errors.append(f"{cid}: L4 is {len(l4)} chars, budget {L4_CHARS[0]}-{L4_CHARS[1]}")
    if l4.count(memory) < 2:
        errors.append(f"{cid}: L4 must state the memory point twice")
    if main_hex not in l4:
        errors.append(f"{cid}: L4 missing main hex")
    if not l4.startswith("FORMAT:"):
        errors.append(f"{cid}: L4 must start with FORMAT")
    early, _, late = l4.partition("\nEXCLUDE:")
    if "Early exclude:" not in early[:500]:
        errors.append(f"{cid}: L4 needs an early exclude in the first block")
    if "EXCLUDE:" not in l4:
        errors.append(f"{cid}: L4 missing final EXCLUDE")
    for item in ch["banned"]:
        if item not in late:
            errors.append(f"{cid}: banned item not in L4 exclude: {item}")
    if production in l4:
        errors.append(f"{cid}: L4 pasted the L3 production paragraph")
    errors.extend(_deny_scan(f"{cid} L4", l4))
    return errors
