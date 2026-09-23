#!/usr/bin/env python3
"""Generate gdextension_interface.h from Godot's gdextension_interface.json.

Godot 4.7 does not ship the C header; it is generated from
core/extension/gdextension_interface.json. This script emits the declarations
the SceneScore extension compiles against.
"""

import json
import sys


def pascal(snake: str) -> str:
    return "".join(part[:1].upper() + part[1:] for part in snake.split("_") if part)


def strip_cv(type_name: str) -> str:
    t = type_name.strip()
    t = t.replace("const ", "").replace("const", "")
    while t.endswith("*"):
        t = t[:-1].strip()
    return t


def emit_function(type_info: dict) -> str:
    name = type_info["name"]
    ret = (type_info.get("return_value") or {}).get("type", "void")
    args = type_info.get("arguments") or []
    if name == "GDExtensionInterfaceFunctionPtr":
        return "typedef void (*GDExtensionInterfaceFunctionPtr)();"
    if not args:
        arg_s = "void"
    else:
        parts = []
        for i, arg in enumerate(args):
            arg_name = arg.get("name") or f"arg{i}"
            parts.append(f"{arg['type']} {arg_name}")
        arg_s = ", ".join(parts)
    return f"typedef {ret} (*{name})({arg_s});"


def emit_interface(item: dict) -> str:
    name = "GDExtensionInterface" + pascal(item["name"])
    ret = (item.get("return_value") or {}).get("type", "void")
    args = item.get("arguments") or []
    if not args:
        arg_s = "void"
    else:
        arg_s = ", ".join(f"{arg['type']} {arg['name']}" for arg in args)
    return f"typedef {ret} (*{name})({arg_s});"


def struct_deps(type_info: dict, structs: set) -> list:
    deps = []
    for member in type_info["members"]:
        raw = member["type"].strip()
        if "*" in raw:
            continue
        base = strip_cv(raw)
        if base in structs and base != type_info["name"]:
            deps.append(base)
    return deps


def main() -> int:
    src = sys.argv[1]
    dst = sys.argv[2]
    data = json.load(open(src, encoding="utf-8"))
    types = data["types"]
    structs = {t["name"] for t in types if t["kind"] == "struct"}

    lines = []
    a = lines.append
    a("/* Generated from Godot 4.7 core/extension/gdextension_interface.json.")
    a(" * Declarations follow the Godot Engine GDExtension C ABI (MIT). */")
    a("#pragma once")
    a("#ifndef __cplusplus")
    a("#include <stddef.h>")
    a("#include <stdint.h>")
    a("#if !defined(__wchar_t_defined) && !defined(_WCHAR_T)")
    a("typedef int wchar_t;")
    a("#define __wchar_t_defined")
    a("#endif")
    a("typedef uint32_t char32_t;")
    a("typedef uint16_t char16_t;")
    a("#else")
    a("#include <cstddef>")
    a("#include <cstdint>")
    a("extern \"C\" {")
    a("#endif")
    a("")

    for t in types:
        if t["kind"] == "handle":
            ptr = "const void *" if t.get("is_const") else "void *"
            a(f"typedef {ptr}{t['name']};")
    a("")
    for t in types:
        if t["kind"] == "alias" and t["type"] not in structs:
            a(f"typedef {t['type']} {t['name']};")
    a("")
    for t in types:
        if t["kind"] != "enum":
            continue
        a("typedef enum {")
        for value in t["values"]:
            a(f"\t{value['name']} = {value['value']},")
        a(f"}} {t['name']};")
        a("")
    for name in sorted(structs):
        a(f"typedef struct {name} {name};")
    a("")
    for t in types:
        if t["kind"] == "function":
            a(emit_function(t))
    a("")

    pending = [t for t in types if t["kind"] == "struct"]
    emitted = set()
    guard = 0
    while pending and guard < 100:
        guard += 1
        next_pending = []
        for t in pending:
            deps = struct_deps(t, structs)
            if any(dep not in emitted for dep in deps):
                next_pending.append(t)
                continue
            a(f"struct {t['name']} {{")
            for member in t["members"]:
                a(f"\t{member['type']} {member['name']};")
            a("};")
            a("")
            emitted.add(t["name"])
        if len(next_pending) == len(pending):
            raise SystemExit("struct dependency cycle: " + ", ".join(t["name"] for t in pending))
        pending = next_pending

    for t in types:
        if t["kind"] == "alias" and t["type"] in structs:
            a(f"typedef {t['type']} {t['name']};")
    a("")
    for item in data["interface"]:
        a(emit_interface(item))
    a("")
    a("#ifdef __cplusplus")
    a("}")
    a("#endif")
    a("")
    with open(dst, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
    print(f"wrote {dst} ({len(lines)} lines)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
