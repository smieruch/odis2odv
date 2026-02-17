<!--
Licensed under CC BY 4.0
https://creativecommons.org/licenses/by/4.0/
-->

# ODIS Profile: ODV Generic Spreadsheet Mapping

**Profile ID:** odis-profile-odv-generic-spreadsheet  
**Status:** Draft  
**Version:** 0.1

---

## Overview

This profile defines a **web-first pattern** for publishing ocean datasets so they can be **automatically converted into ODV Generic Spreadsheets**.

It combines:
- **schema.org** for dataset discovery and access, and
- a small **`odv:` extension vocabulary** to describe deterministic conversion rules.

The profile targets data providers who publish tabular data but do not natively produce ODV files, and services that generate ODV outputs programmatically.

---

## Scope

### In scope
- Dataset landing pages with embedded JSON-LD
- Tabular provider data (e.g. CSV, TSV)
- Deterministic generation of ODV Generic Spreadsheets

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
- **Extension vocabularies** for domain-specific requirements

ODIS harvesters can index datasets conforming to this profile **without understanding the `odv:` extension**.

---

## Vocabularies Used

### schema.org (Required)

The following schema.org terms are REQUIRED:

- Dataset
- distribution
- DataDownload
- contentUrl

These terms support dataset discovery, indexing, and access.

### ODV Extension Vocabulary (`odv:`)

A dedicated namespace provides conversion-specific semantics:

https://<your-domain>/ns/odv#


The `odv:` vocabulary:
- is procedural and deterministic,
- is consumed by conversion tools,
- does not affect ODIS discovery.

Documentation and versioning of the vocabulary are maintained separately.

---

## Required Metadata

### Dataset-Level (schema.org)

A conforming dataset MUST include:

- `@type: Dataset`
- `name`
- `description`
- `distribution` with at least one `DataDownload`
- `distribution.contentUrl` pointing to a downloadable source file

---

### Source Description (`odv:source`)

Describes how the provider file is parsed.

**Required**
- `odv:delimiter`

**Optional**
- `odv:missingValues`

---

### Target Description (`odv:odvSpreadsheet`)

Describes the ODV output to be generated.

**Required**
- `odv:outputFormat` (currently `generic-spreadsheet`)
- `odv:separator`
- `odv:columns` (ordered)

**Optional**
- `odv:versionTag`
- `odv:encodingTag`

---

### Column Mapping (`odv:columns`)

Each entry defines **one output column** in the generated ODV spreadsheet.

**Required per column**
- `odv:odvLabel`
- `odv:role` (`meta` or `data`)
- `odv:value_type`

**Conditional rules**
- Meta columns MUST include `odv:var_type`
- Synthetic columns (`odv:sourceColumn = null`) MUST include `odv:defaultIfMissing`
- Column order is significant and preserved

---

## Processing Model (Informative)

A conforming generator performs the following steps:

1. Validate JSON-LD against the profile’s JSON Schema
2. Retrieve the source file via `distribution.contentUrl`
3. Parse the file using `odv:source`
4. Generate ODV header metadata
5. Emit the ODV column label line
6. Populate data rows according to `odv:columns`

The result MUST be a valid ODV Generic Spreadsheet.

---

## Validation and Conformance

Conformance requires:

- Valid JSON syntax
- Successful validation against the accompanying JSON Schema
- Presence of all required schema.org and `odv:` elements

Validation occurs **prior to data conversion**.

---

## Example

A complete example conforming to this profile is available at:

https://<your-domain>/ns/odv/examples/ocean-data-test-001.json


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
- Clear separation of discovery metadata and execution rules
- Reuse across providers and tooling

It provides a practical bridge between **web-based metadata publication** and **ODV-based ocean data analysis**.
