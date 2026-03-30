<!--
Licensed under CC BY 4.0
https://creativecommons.org/licenses/by/4.0/
-->

# ODIS Profile: ODV Generic Spreadsheet Mapping

**Profile ID:** odis-profile-odv-generic-spreadsheet  
**Status:** Draft  
**Version:** 0.2

---

## Overview

This profile defines a **web-first pattern** for publishing ocean datasets so they can be **automatically converted into ODV Generic Spreadsheets**.

It uses:
- **schema.org** for dataset discovery and access, and
- **schema.org-native patterns** (`variableMeasured` + `PropertyValue` + `additionalProperty`) to describe deterministic conversion rules.

The profile targets data providers who publish tabular data but do not natively produce ODV files, and services that generate ODV outputs programmatically.

---

## Scope

### In scope
- Dataset landing pages with embedded JSON-LD
- Tabular provider data (e.g. CSV, TSV)
- Deterministic generation of ODV Generic Spreadsheets using mapping metadata embedded in JSON-LD

### Out of scope
- Scientific validation of values
- Quality flag semantics
- ODV Collections or NetCDF generation (future extensions)

---

## Alignment with ODIS Patterns

This profile follows established ODIS patterns:

- **Dataset Landing Pages** with embedded JSON-LD
- **schema.org/Dataset** for discovery
- **distribution.contentUrl** for machine-accessible downloads
- Use of **schema.org extension points** (`additionalProperty`) for domain-specific requirements

ODIS harvesters can index datasets conforming to this profile **without understanding the mapping details**.

---

## Vocabularies Used

### schema.org (Required)

The profile uses only schema.org terms.

Core required terms:

- Dataset
- distribution
- DataDownload
- contentUrl
- variableMeasured
- PropertyValue
- additionalProperty

These terms support dataset discovery, indexing, access, and mapping metadata.

---

## Required Metadata

### Dataset-Level (schema.org)

A conforming dataset MUST include:

- `@type: Dataset`
- `@id` (a resolvable identifier for the JSON-LD resource)
- `identifier` (a landing page URL intended for humans)
- `name`
- `description`
- `distribution` with at least one `DataDownload`
- `distribution.contentUrl` pointing to a downloadable **source data file** (CSV/TSV/etc.)

**Provider guidance (informative):**  
Longitude and latitude are mandatory for ODV conversion; records without geographic coordinates cannot be converted.

---

## Global Conversion Hints (schema.org / additionalProperty)

Dataset-level conversion hints are expressed using:

- `Dataset.additionalProperty[]` entries of type `PropertyValue`

The following dataset-level hints are RECOMMENDED:

- `primaryVariableTargetColumn`  
  - **Value:** the name of the output column to be treated as the primary ODV variable (e.g., Pressure)
- `columnSeparator`  
  - **Value:** column delimiter used in the source file (e.g., `\t`, `,`, `;`)
- `fillValue`  
  - **Value:** a provider fill value/missing value marker (any literal value)

All dataset-level hints are optional unless a converter requires them.

---

## Column Mapping Model

### Mapping container (schema.org)

Column mappings are expressed as:

- `Dataset.variableMeasured[]` entries of type `PropertyValue`

Each `PropertyValue` represents **one source column**.

**Required per source column**
- `name`  
  - the exact header/name of the source column

**Recommended per source column**
- `additionalProperty[]` includes an entry:
  - `PropertyValue(name="targetColumn", value="<ODV output header>")`

This expresses a deterministic mapping from the source column to the ODV output column.

### Optional per-column hints

Per-source-column optional hints MAY be attached via `additionalProperty[]`:

- `dataType`  
  - Value: one of `Text`, `Number`, `DateTime` (string values), when needed
- `role`  
  - Value: `meta` or `data`, when applicable
- `P01`  
  - Value: NERC P01 URI for the variable (e.g., longitude/latitude terms)

### Human-readable descriptions (recommended)

If a column needs a human-facing explanation, use:

- `PropertyValue.description`

This profile discourages inventing additional ad-hoc “comment” properties when `description` is sufficient.

---

## Processing Model (Informative)

A conforming generator performs the following steps:

1. Parse JSON-LD and locate the `Dataset`
2. Retrieve the source file via `distribution.contentUrl`
3. Apply dataset-level parsing hints:
   - `columnSeparator` (if provided)
   - `fillValue` (if provided)
4. Build an ordered output column list based on `targetColumn` mappings and converter defaults
5. Map source fields to output fields using `variableMeasured`:
   - For each source column `name`, write to its `targetColumn`
6. Apply converter rules for derived/created fields not present in the source (implementation-specific)
7. Emit a valid ODV Generic Spreadsheet

---

## Validation and Conformance

Conformance requires:

- Valid JSON syntax
- Presence of all required schema.org terms above
- Deterministic mappings via `variableMeasured` and `additionalProperty(targetColumn)`

Validation should occur **prior to data conversion**.

---

## Example

A complete example conforming to this profile is expected to include:

- `@id`, `identifier`
- `distribution.contentUrl`
- dataset-level `additionalProperty` (e.g., `primaryVariableTargetColumn`, `columnSeparator`, `fillValue`)
- `variableMeasured` mappings from source columns to ODV output columns

---

## Extensibility

The profile may be extended to support:

- Quality flag columns
- Multiple source files
- Unit conversion rules
- Additional input formats (e.g. NetCDF)

Extensions MUST NOT change the semantics of existing terms.

---

## Summary

This profile enables:

- ODIS-compatible dataset publication
- Deterministic generation of ODV Generic Spreadsheets
- Clear separation of discovery metadata and mapping metadata
- Reuse across providers and tooling

It provides a practical bridge between **web-based metadata publication** and **ODV-based ocean data analysis**.

