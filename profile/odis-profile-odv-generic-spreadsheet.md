<!--
Licensed under CC BY 4.0
https://creativecommons.org/licenses/by/4.0/
-->

# ODIS Profile: ODV Generic Spreadsheet Mapping

**Profile ID:** odis-profile-odv-generic-spreadsheet  
**Status:** Stable  
**Version:** 1.0

---

## Overview

This profile defines a web-first pattern for publishing ocean datasets so they
can be automatically converted into ODV Generic Spreadsheets.

It uses:

- schema.org for dataset discovery and access
- schema.org-native structures (`Dataset`, `DataDownload`, `PropertyValue`,
  `variableMeasured`, `additionalProperty`, `description`, `unitText`,
  `propertyID`)
- ODIS2ODV profile-specific controlled values inside schema.org
  `PropertyValue.name`, for example `targetColumn`, `role`,
  `qualityFlagScheme`, and `relatedColumn`

The profile targets data providers publishing CSV/TSV style tabular datasets and services that
generate ODV-compatible outputs programmatically.

---

## Design Goals

The profile is designed to be:

- deterministic
- FAIR
- machine-readable
- AI-ready
- schema.org-native
- minimal

Every semantic concept is represented exactly once. The profile intentionally avoids duplicate
metadata, custom vocabularies, and inferred mappings wherever possible.

---

## Scope

### In scope

- Dataset landing pages with embedded JSON-LD
- Tabular provider data (CSV, TSV)
- Deterministic generation of ODV Generic Spreadsheets
- ODV collection metadata description
- Source-column to ODV-column mappings
- Auxiliary column descriptions for quality flags and standard deviations

### Out of scope

- Scientific quality control
- Defining new quality flag semantics
- ODV binary collections
- NetCDF generation

---

## ODIS2ODV Dataset

An ODIS2ODV document is represented as a schema.org `Dataset`. The dataset contains the general dataset metadata together with the information required to map the source data to an ODV Generic Spreadsheet.

### schemaVersion

**Required**

The `schemaVersion` property MUST be present and identifies the version of the ODIS2ODV profile used to create the JSON-LD document. It allows software, including the ODIS2ODV converter, to determine which version of the profile the document conforms to.

The value SHOULD follow semantic versioning (e.g. `1.0.0`).

A simple version string is sufficient. A resolvable URL identifying the profile version MAY be used in future profile releases.

**Example**

```json
"schemaVersion": "1.0.0"
```

---

## Alignment with ODIS Patterns

This profile follows ODIS conventions:

- `schema.org/Dataset` for discovery
- `distribution.contentUrl` for machine-accessible source files
- `variableMeasured` for variable descriptions
- `PropertyValue` for measured-variable metadata
- `additionalProperty` as the schema.org extension mechanism for additional
  characteristics where no more specific schema.org property exists

ODIS harvesters can index datasets without understanding the ODV conversion
metadata.

---

## Vocabulary and Profile Terms

The JSON-LD document uses schema.org terms and does not introduce a custom
JSON-LD namespace.

However, some ODIS2ODV concepts are expressed as controlled string values inside
schema.org `PropertyValue.name`. These values are part of this profile, not
native schema.org vocabulary terms.

Examples:

- `targetColumn`
- `unit`
- `unitID`
- `role`
- `qualityFlagScheme`
- `relatedColumn`
- `dataField`
- `dataType`
- `primaryVariableTargetColumn`
- `columnSeparator`
- `columnNameRow`
- `dataStartRow`
- `columnNameRow`
- `dataStartRow`
- `fillValue`

This means ODIS2ODV is best described as **schema.org-based** and
**schema.org-compatible**, using schema.org extension points rather than a custom
ontology.

---

## Dataset discovery

ODIS2ODV-compatible datasets SHOULD include:

```json
"keywords": [
  "ODV-compatible"
]
```

Datasets following this profile SHOULD include the keyword
ODV-compatible in Dataset.keywords to support discovery of ODV-ready
datasets in ODIS catalogues.


## Required Dataset Metadata

A conforming dataset MUST provide:

- `@context`
- `@type: Dataset`
- `@id`
- `identifier`
- `name`
- `distribution`
- `distribution.contentUrl`
- `variableMeasured`

A dataset-level `description` is strongly recommended for ODIS discovery.

The following ODV core target-column mappings are mandatory for successful ODV conversion:

- `Longitude`
- `Latitude`

The following ODV metadata columns are strongly recommended but not mandatory:

- `Cruise`
- `Station`
- `yyyy-mm-ddThh:mm:ss.sss`

The ODV time column is not mandatory,
because ODV can handle datasets without timestamps.

The ODV `Type` column is optional.

If present, `Type` is interpreted as metadata text.

Allowed source data values are:

- `C` = CTD measurement
- `B` = bottle measurement

The converter SHOULD validate that no other values occur.

---

## Dataset-Level ODV Metadata and Conversion Hints

Dataset-level information is encoded as:

`Dataset.additionalProperty[]`

Each entry is a schema.org `PropertyValue`.

### ODV Collection Metadata

#### dataField

Defines the ODV scientific domain.

Supported ODV values:

- `GeneralField`
- `Ocean`
- `Atmosphere`
- `Land`
- `IceSheet`
- `SeaIce`
- `Sediment`

Example:

```json
{
  "@type": "PropertyValue",
  "name": "dataField",
  "value": "Ocean"
}
```

#### dataType

Defines the ODV data organization.

Supported ODV values:

- `GeneralType`
- `Profiles`
- `TimeSeries`
- `Trajectories`

Example:

```json
{
  "@type": "PropertyValue",
  "name": "dataType",
  "value": "Profiles"
}
```

---

## Conversion Hints

### primaryVariableTargetColumn

Defines the ODV primary variable.

For profile datasets this is typically the vertical coordinate, for example
pressure or depth.

Example:

```json
{
  "@type": "PropertyValue",
  "name": "primaryVariableTargetColumn",
  "value": "Pressure"
}
```

### columnSeparator

Defines the source file separator.

Examples:

- tab (`\t`)
- comma
- semicolon

### columnNameRow

**Required**

Defines the 1-based row number containing the source column names.

The converter reads this row using `columnSeparator` and matches its values
against `PropertyValue.name` in `Dataset.variableMeasured`.

Example:

```json
{
  "@type": "PropertyValue",
  "name": "columnNameRow",
  "value": 5
}
```

### dataStartRow

**Required**

Defines the 1-based row number of the first source-data record.

Rows between `columnNameRow` and `dataStartRow` are skipped. Unit rows or other
provider-specific header rows are not parsed by the converter because unit
information is encoded in `unitText`, `unit`, and `unitID`.

`dataStartRow` MUST be greater than `columnNameRow`.

The JSON Schema validates that both row properties are positive integers. The
relationship `dataStartRow > columnNameRow` MUST additionally be checked by the
converter because the values occur in separate `PropertyValue` array entries.

Example:

```json
{
  "@type": "PropertyValue",
  "name": "dataStartRow",
  "value": 7
}
```

### fillValue

Defines the missing-value marker.

`fillValue` is optional.

The following dataset-level entries are required:

- `dataField`
- `dataType`
- `primaryVariableTargetColumn`
- `columnSeparator`

The following dataset-level entries are optional:

- `fillValue`
- `timeZone`

`timeZone` specifies the time zone of source timestamps when the source data does not already
contain an explicit UTC offset. The value **SHALL** be a standard **IANA time-zone identifier**, for
example `UTC`, `Pacific/Honolulu`, or `Europe/Berlin`.

When `timeZone` is present, the converter **SHALL** interpret all source timestamps in the specified time
zone and convert them to UTC before generating the ODV timestamp column (`yyyy-mm-ddThh:mm:ss.sss`).

If `timeZone` is omitted, the converter **SHALL** preserve the source timestamps without timezone
conversion. In this case, the timezone of the timestamps is considered unknown.

---

## Column Mapping Model

Column mappings are represented through:

`Dataset.variableMeasured[]`

Each item is a schema.org `PropertyValue` and represents one source column.

Required:

- `PropertyValue.name`

`PropertyValue.name` MUST contain the exact column header as it appears in the source data table. In
this profile, `name` therefore serves as the explicit source-column identifier. A separate
`sourceColumn` property is not required.

The source-column name is intentionally kept in the native schema.org `name` property to avoid
duplicating the same information in an additional profile-specific `PropertyValue`.


For regular data and metadata columns, the ODV output variable is defined by:

`additionalProperty(name="targetColumn")`

Example:

```json
{
  "@type": "PropertyValue",
  "name": "Lon",
  "description": "Longitude of the sampling location.",
  "unitText": "degrees east",
  "propertyID": "https://vocab.nerc.ac.uk/collection/P01/current/ALONZZ01/",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "targetColumn",
      "value": "Longitude"
    }
  ]
}
```

---

## Native schema.org Column Metadata

Columns SHOULD use native schema.org properties where available.

### name

Use `PropertyValue.name` for the exact source-column header.

Within `Dataset.variableMeasured`, `name` is not merely a descriptive variable label. It is the machine-actionable identifier used to locate the corresponding column in the source table. Human-readable scientific meaning SHOULD be provided through `description`, while persistent semantic meaning SHOULD be provided through `propertyID`.

### description

Use `PropertyValue.description` for human-readable variable definitions,
comments, explanations, or method notes.

### unitText

Use `PropertyValue.unitText` for the concise, human-readable name of the
physical unit, for example `degrees Celsius`, `decibar`, or
`Number per millilitre`.

`unitText` is optional. If the source column has no physical unit, `unitText`
SHOULD be omitted rather than set to an empty string. If present, `unitText`
SHALL contain the unit name itself, not a sentence describing unit provenance,
conversion, or display conventions. Such explanatory information belongs in
`description`.

`unitText` is informational schema.org metadata. The ODV converter MUST NOT
derive ODV output column names or ODV unit labels from `unitText`.

The three unit-related fields have distinct purposes:

- `unitText`: human-readable unit name
- `unit`: compact unit label used in the generated ODV Generic Spreadsheet
- `unitID`: persistent machine-readable unit identifier, preferably a NERC P06 URI

For columns without a physical unit, all three fields SHOULD be omitted. For
quantitative columns with a physical unit, all three SHOULD normally be
provided when the information is available. Legacy or incomplete source
metadata may provide only a subset.

Example:

- `targetColumn`: `Temperature`
- `unitText`: `degrees Celsius`
- `unit`: `degC`
- `unitID`: `https://vocab.nerc.ac.uk/collection/P06/current/UPAA/`

If the source notation differs from the normalized ODV label, the distinction
SHOULD be explained in `description`, for example: `The original source unit is
degrees Celsius; the ODV target uses the compact unit label degC.`

### propertyID

Use `PropertyValue.propertyID` for persistent semantic identifiers such as NERC
P01 URIs.

Example:

```json
"propertyID": "https://vocab.nerc.ac.uk/collection/P01/current/TEMPPR01/"
```

Free-text names SHOULD NOT be placed in `propertyID`; use `description` instead.

---

## ODIS2ODV Per-Column Metadata

Per-column conversion metadata is expressed through
`PropertyValue.additionalProperty[]`.

### dataType

Allowed values:

- `Text`
- `Number`
- `DateTime`

### role

Allowed values:

- `meta`
- `data`
- `quality`
- `standardDeviation`
- `timeComponent`

Regular ODV columns use:

- `role = meta`
- `role = data`

Auxiliary columns use:

- `role = quality`
- `role = standardDeviation`

### targetColumn

Required for regular ODV data and metadata columns.

It defines the ODV output variable name without a unit suffix. Units are described
separately with `unit` and `unitID`.

### unit

Defines the compact unit label used in the generated ODV Generic Spreadsheet,
for example `degC`, `dbar`, `degrees_east`, or `1/ml`.

The value may differ from the human-readable `unitText` label. For example,
`unitText = Number per millilitre` may be paired with `unit = 1/ml`.

`unit` SHOULD normally be provided for quantitative variables with a physical
unit. If no physical unit applies, `unit` SHOULD be omitted rather than set to
an empty string. For genuinely dimensionless quantitative variables, a suitable
label such as `1` MAY be used. It is normally omitted for textual metadata,
timestamps, and quality flags.

### unitID

Provides a persistent machine-readable identifier for the unit. A resolvable
vocabulary URI SHOULD be used, preferably a NERC P06 URI where available.

`unitID` SHOULD normally accompany `unit` for quantitative variables when a
suitable persistent identifier is available. It improves semantic
interoperability and AI-readiness, but it does not replace either the
human-readable `unitText` value or the compact ODV `unit` value. If no physical
unit applies, `unitID` SHOULD be omitted.


Example for abundance data:

```json
{
  "@type": "PropertyValue",
  "name": "ProchlBact",
  "description": "Prochlorococcus abundance. The original source notation is #/ml; the ODV target uses the compact unit label 1/ml.",
  "unitText": "Number per millilitre",
  "propertyID": "https://vocab.nerc.ac.uk/collection/P01/current/...",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "targetColumn",
      "value": "ProchlBact"
    },
    {
      "@type": "PropertyValue",
      "name": "unit",
      "value": "1/ml"
    },
    {
      "@type": "PropertyValue",
      "name": "unitID",
      "value": "https://vocab.nerc.ac.uk/collection/P06/current/UCML/"
    }
  ]
}
```

The following target columns are mandatory:

- `Longitude`
- `Latitude`

The following target columns are strongly recommended but optional:

- `Cruise`
- `Station`
- `yyyy-mm-ddThh:mm:ss.sss`

The following target column is optional:

- `Type`

For `targetColumn = Type`, allowed source data values are:

- `C` = CTD measurement
- `B` = bottle measurement

The converter SHOULD validate these values during data conversion.

For known ODV metadata target columns such as `Cruise`, `Station`,
`Longitude`, `Latitude`,
`yyyy-mm-ddThh:mm:ss.sss`, and `Type`, converters may infer `role = meta`
and the appropriate datatype. Therefore `role` and `dataType` do not need to
be repeated for these known ODV columns.

### relatedColumn

Required for auxiliary columns with `role = quality` or `role = standardDeviation`.

`relatedColumn` contains the final ODV target variable name of the measured
variable to which the auxiliary column belongs.

It MUST refer to the `targetColumn` value, not to the original source column name or to a unit-qualified label.

Example:

```json
{
  "@type": "PropertyValue",
  "name": "Temperature_sd",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "role",
      "value": "standardDeviation"
    },
    {
      "@type": "PropertyValue",
      "name": "relatedColumn",
      "value": "Temperature"
    }
  ]
}
```

### qualityFlagScheme

Required for quality flag columns.

It contains an ODV-supported quality flag set name, for example:

- `ARGO`
- `WOCEBOTTLE`
- `SEADATANET`
- `QARTOD`
- `PANGAEA`

---



For known ODV core metadata columns:

- `Cruise`
- `Station`
- `Longitude`
- `Latitude`
- `yyyy-mm-ddThh:mm:ss.sss`
- `Type`

the converter infers:

- `role`
- `dataType`

Therefore these properties do not need to be provided in the JSON-LD mapping.

## Timestamp Assembly from Separate Columns

Many provider tables do not contain a single ISO 8601 timestamp column.
Instead, time information may be stored in separate columns such as:

- `Year`
- `Month`
- `Day`
- `Hour`
- `Minute`
- `Second`

or as separate `Date` and `Time` columns.

ODIS2ODV supports this using `role = timeComponent`.

Columns with `role = timeComponent` are used to assemble the ODV timestamp column:

`yyyy-mm-ddThh:mm:ss.sss`

Therefore `targetColumn` is not required for time-component columns.
Each time-component column only needs to declare which part of the timestamp
it represents using `dateTimeComponent`.

Allowed `dateTimeComponent` values:

- `year`
- `month`
- `day`
- `hour`
- `minute`
- `second`
- `millisecond`
- `date`
- `time`

When `dateTimeComponent = date`, supported source-column formats are:

- `yyyy-mm-dd`
- `yyyy-mm`

When `dateTimeComponent = time`, supported source-column formats are:

- `hh:mm:ss.sss`
- `hh:mm:ss`
- `hh:mm`

Example:

```json
{
  "@type": "PropertyValue",
  "name": "Year",
  "description": "Year component of the observation date.",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "role",
      "value": "timeComponent"
    },
    {
      "@type": "PropertyValue",
      "name": "dateTimeComponent",
      "value": "year"
    }
  ]
}
```

If a direct source column already maps to `yyyy-mm-ddThh:mm:ss.sss`, converters
SHOULD use the direct mapping and do not need time-component assembly.


## Auxiliary Column Examples

### Quality flag column

```json
{
  "@type": "PropertyValue",
  "name": "Temperature Flag",
  "description": "Quality flag for the temperature observation.",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "role",
      "value": "quality"
    },
    {
      "@type": "PropertyValue",
      "name": "qualityFlagScheme",
      "value": "ARGO"
    },
    {
      "@type": "PropertyValue",
      "name": "relatedColumn",
      "value": "Temperature"
    }
  ]
}
```

### Standard-deviation column

```json
{
  "@type": "PropertyValue",
  "name": "Temperature std",
  "description": "Standard deviation of the temperature observation.",
  "unitText": "degrees Celsius",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "unit",
      "value": "degC"
    },
    {
      "@type": "PropertyValue",
      "name": "unitID",
      "value": "https://vocab.nerc.ac.uk/collection/P06/current/UPAA/"
    },
    {
      "@type": "PropertyValue",
      "name": "role",
      "value": "standardDeviation"
    },
    {
      "@type": "PropertyValue",
      "name": "relatedColumn",
      "value": "Temperature"
    }
  ]
}
```

Auxiliary columns do not require `targetColumn` because their relationship to the
measured variable is expressed through `relatedColumn`.

---


## Metadata Validation vs Data Validation

The JSON Schema validates the JSON-LD mapping metadata.

It can validate values that are present in the JSON-LD document, for example:

- `dataField`
- `dataType`
- `role`
- `qualityFlagScheme`
- `dateTimeComponent`
- `targetColumn`

It cannot validate values that occur only in the source data table, for example:

- `Type` column values (`C` or `B`)
- longitude and latitude numeric ranges
- timestamp component formats
- quality flag values

These checks are part of converter-level data validation.

---

## Processing Model

### Source Data Assumptions

ODIS2ODV assumes that the source dataset is represented as a
rectangular, column-oriented table in which each row corresponds to
one observation.

Rows belonging to the same logical observation unit SHALL appear as
one contiguous block in the source table.

Within each logical observation unit, observations SHALL be ordered by
the declared primary variable (`primaryVariableTargetColumn`).

Converters MAY validate these assumptions and report violations as data
validation errors. Converters SHOULD NOT silently reorder observations unless
explicitly requested.

### Processing

A converter SHOULD:

1. Validate JSON-LD against the ODIS2ODV JSON Schema
2. Read the source file from `distribution.contentUrl`
3. Apply dataset parsing metadata:
   - read column names from the 1-based `columnNameRow`
   - begin reading observations at the 1-based `dataStartRow`
   - use `columnSeparator`
   - apply `fillValue` if present
   - skip rows between `columnNameRow` and `dataStartRow`
4. Extract source-to-target mappings from `variableMeasured`
5. Map `role = meta` and `role = data` columns to ODV output columns using
   `targetColumn`
6. If needed, assemble `yyyy-mm-ddThh:mm:ss.sss` from `role = timeComponent`
   columns using `dateTimeComponent`; `targetColumn` is inferred for these columns
7. Attach auxiliary columns with `role = quality` or `role = standardDeviation` to their
   `relatedColumn`
8. Apply ODV-specific requirements
9. Emit an ODV Generic Spreadsheet

The converter MUST NOT rely on provider-specific APIs, web pages, or hidden
conventions. Provider-specific enrichment may happen before conversion, but the
resulting JSON-LD document MUST contain all information required for
deterministic conversion.

---

## Validation

Validation requires:

- valid JSON syntax
- JSON Schema Draft 2020-12 validation
- presence of required schema.org metadata
- deterministic column mappings
- valid auxiliary-column relationships

Recommended command:

```bash
./jsonValidate.bash
```

---

## Repository Components

- `schema/odv-odis2odv.schema.json`  
  Machine-readable JSON Schema

- `profile/odis-profile-odv-generic-spreadsheet.md`  
  Human-readable ODIS profile

- `examples/`  
  Valid example datasets

- `tools/`  
  Validation and conversion utilities

---

## Extensibility

Future extensions may support:

- multiple source files
- unit conversion rules
- additional source formats
- additional auxiliary-column roles

Extensions MUST preserve existing semantics.

---

## Summary

This profile enables:

- ODIS-compatible dataset publication
- schema.org-compatible JSON-LD metadata
- reproducible ODV Generic Spreadsheet generation
- clear separation between discovery metadata, ODV metadata,
  conversion rules, and auxiliary-column relationships
