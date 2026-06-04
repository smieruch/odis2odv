#!/usr/bin/env python3
"""Validate a JSON(-LD) instance against a JSON Schema.

Usage:
  python jsonValidate_updated.py odv-odis2odv.schema.updated.json ocean-data-test-003.json
"""

import argparse
import json
import sys
from pathlib import Path

from jsonschema import Draft202012Validator, exceptions


def load_json(path: Path):
    try:
        with path.open("r", encoding="utf-8") as f:
            return json.load(f)
    except FileNotFoundError:
        raise SystemExit(f"ERROR: File not found: {path}")
    except json.JSONDecodeError as e:
        raise SystemExit(f"ERROR: Invalid JSON in {path}: line {e.lineno}, column {e.colno}: {e.msg}")


def format_path(error: exceptions.ValidationError) -> str:
    if not error.absolute_path:
        return "$"
    return "$" + "".join(f"[{repr(part)}]" if isinstance(part, int) else f".{part}" for part in error.absolute_path)


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate JSON against a JSON Schema.")
    parser.add_argument("schema", type=Path, help="Path to JSON Schema file")
    parser.add_argument("instance", type=Path, help="Path to JSON instance file")
    args = parser.parse_args()

    schema = load_json(args.schema)
    instance = load_json(args.instance)

    try:
        Draft202012Validator.check_schema(schema)
    except exceptions.SchemaError as e:
        print(f"ERROR: Schema is invalid: {e.message}", file=sys.stderr)
        return 2

    validator = Draft202012Validator(schema)
    errors = sorted(validator.iter_errors(instance), key=lambda e: list(e.absolute_path))

    if not errors:
        print(f"Valid: {args.instance} conforms to {args.schema}")
        return 0

    print(f"Invalid: {len(errors)} validation error(s) found\n")
    for i, error in enumerate(errors, start=1):
        print(f"{i}. {format_path(error)}")
        print(f"   {error.message}\n")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
