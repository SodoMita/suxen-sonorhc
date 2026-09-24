"""Assemble generator prompts from a character level record.

L4 is a compression of the level docs, not a second design. Construction
detail that lives only in L3 is judged by a person; it is not pasted into
the prompt, because truncation cuts the end of a long prompt and that end
is the exclusion block.
"""

from __future__ import annotations

EARLY_EXCLUDE = (
    "no text, no watermark, no extra limbs, no nudity, "
    "no transparent clothing, no sexual pose, fully clothed"
)


def _zones(ch: dict) -> dict:
    return ch["palette"]["zones"]


def _join(items: list[str]) -> str:
    return "; ".join(items)


def _cap(text: str) -> str:
    text = text.strip()
    if text and text[0].islower():
        return text[0].upper() + text[1:]
    return text


def sprite_prompt(doc: dict, ch: dict) -> str:
    """Default standing-sprite prompt. This is level L4.

    Kept under the character budget on purpose. Garment construction stays
    in L3; pasting it here is how exclusions get truncated off.
    """
    main = ch["palette"]["main"]
    zones = _zones(ch)
    banned = _join(ch["banned"])
    props = ", ".join(ch["props"])
    blocks = [
        (
            "FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey "
            "background, head to soles, margin. "
            f"Early exclude: {EARLY_EXCLUDE}."
        ),
        "STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.",
        (
            f"IDENTITY: {ch['name']}. {_cap(ch['role'])}. "
            f"Class: {ch['silhouette']['class']}. "
            f"{ch['silhouette']['glance']} "
            f"Eyes: {ch['symbol_set']['eye']} "
            f"Hair: {ch['symbol_set']['hair']} "
            f"Defining feature: {ch['memory_point']}."
        ),
        (
            f"WARDROBE: {_cap(ch['wardrobe']['default']['name'])}. "
            f"{_cap(ch['wardrobe']['default']['glance']).rstrip('.')}. "
            f"Props: {props}."
        ),
        (
            f"COLOR: Main {main['name']} {main['hex']}. "
            f"Hair {zones['hair']['hex']}. Eyes {zones['eye']['hex']}. "
            f"Outer {zones['tops1']['hex']}. Accent {zones['decoration1']['hex']}."
        ),
        (
            f"STAGING: {ch['staging']['pose']} "
            "Neutral, alert, eyes to camera. "
            f"{ch['staging']['light']} "
            f"Focal point: {ch['memory_point']}, fully visible."
        ),
        f"EXCLUDE: {doc['global_exclude']} Also: {banned}.",
    ]
    return "\n".join(blocks)


def stage_prompt(doc: dict, ch: dict, stage: str, expression: str | None = None) -> str:
    stage = stage.replace("-", "_")
    if stage in ("sprite", "l4"):
        return sprite_prompt(doc, ch)
    if stage == "turnaround":
        return _turnaround(doc, ch)
    if stage == "plate_white":
        return _plate(ch, "pure white #FFFFFF")
    if stage == "plate_black":
        return _plate(ch, "pure black #000000")
    if stage == "expression":
        if not expression:
            raise SystemExit("expression stage needs --expression")
        return _expression(ch, expression)
    if stage == "hero":
        return _hero(doc, ch)
    known = "sprite, turnaround, plate-white, plate-black, expression, hero"
    raise SystemExit(f"unknown stage {stage!r}; expected one of: {known}")


def _identity_short(ch: dict) -> str:
    return (
        f"{ch['name']}. Silhouette: {ch['silhouette']['class']}. "
        f"Defining feature: {ch['memory_point']}. "
        f"Wearing {ch['wardrobe']['default']['name']}: "
        f"{ch['wardrobe']['default']['glance']}"
    )


def _turnaround(doc: dict, ch: dict) -> str:
    return "\n".join([
        (
            "FORMAT: Character turnaround sheet, four views in one row "
            "(front, three-quarter, side, back) on a complex in-world "
            "background that matches the character, even light, one shared "
            "height line. Do not use a flat green screen. "
            f"Early exclude: {EARLY_EXCLUDE}."
        ),
        f"STYLE: {doc['style_bible']['render']}.",
        f"IDENTITY: {_identity_short(ch)}",
        (
            f"WARDROBE: {ch['wardrobe']['default']['design']} "
            "Same costume and proportions in all four views."
        ),
        (
            f"COLOR: Main {ch['palette']['main']['name']} "
            f"{ch['palette']['main']['hex']}."
        ),
        (
            "STAGING: Relaxed A-pose, arms slightly away from the body. "
            f"The {ch['memory_point']} is clearly visible in every view."
        ),
        f"EXCLUDE: {doc['global_exclude']} Also exclude: {_join(ch['banned'])}.",
    ])


def _plate(ch: dict, background: str) -> str:
    return "\n".join([
        (
            f"FORMAT: Edit the approved sprite onto {background}. "
            "Same canvas. Early exclude: no text, no extra limbs, fully clothed."
        ),
        (
            "KEEP: pose, position, scale, framing, lighting, colour, costume, "
            f"and {ch['memory_point']}. Identical registration."
        ),
        (
            "CHANGE: background only. Composite honestly. Wherever the figure "
            "is not opaque, including hair edges and any glow, the new "
            "background shows through. Do not flatten the figure onto the plate."
        ),
        f"EXCLUDE: {EARLY_EXCLUDE}. Do not redraw the character.",
    ])


def _expression(ch: dict, expression: str) -> str:
    spec = ch["expressions"].get(expression)
    if spec is None:
        known = ", ".join(ch["expressions"])
        raise SystemExit(f"no expression {expression!r} on {ch['id']}; known: {known}")
    return "\n".join([
        (
            "FORMAT: Edit the approved sprite. Change only the face. "
            "Early exclude: no text, fully clothed, no new pose."
        ),
        (
            "KEEP exactly: pose, body, hands, costume, props, hair silhouette, "
            "colours, lighting, canvas size, figure position and scale, and "
            f"{ch['memory_point']}."
        ),
        (
            f"CHANGE: expression to {expression}. Read: {spec['read']} "
            f"Brow: {spec['brow']} Eyes: {spec['eye']} Mouth: {spec['mouth']}"
        ),
        (
            "The head must not move, rotate, tilt, or change size. "
            "Pixels outside the face stay identical."
        ),
        f"EXCLUDE: {EARLY_EXCLUDE}. Also exclude: {_join(ch['banned'])}.",
    ])


def _hero(doc: dict, ch: dict) -> str:
    return "\n".join([
        (
            "FORMAT: Character key visual, tall portrait, finished illustration, "
            "in-world background softer than the figure. Not a runtime sprite. "
            f"Early exclude: {EARLY_EXCLUDE}."
        ),
        f"STYLE: {doc['style_bible']['render']}. Cinematic light, shallow depth of field.",
        f"IDENTITY: {_identity_short(ch)}",
        f"WARDROBE: {ch['wardrobe']['default']['design']}",
        (
            f"COLOR: Main {ch['palette']['main']['name']} "
            f"{ch['palette']['main']['hex']}. Separate the figure from the "
            "background by value."
        ),
        (
            f"STAGING: {ch['staging']['pose']} One strong diagonal. "
            f"The {ch['memory_point']} catches the key light and is the focal point. "
            "Face has the cleanest light in the frame."
        ),
        f"EXCLUDE: {doc['global_exclude']} Also exclude: {_join(ch['banned'])}.",
    ])
