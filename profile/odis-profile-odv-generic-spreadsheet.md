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

The profile targets data providers publishing CSV/TSV style tabular datasets
and services that generate ODV-compatible outputs programmatically.

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
- `role`
- `qualityFlagScheme`
- `relatedColumn`
- `dataField`
- `dataType`
- `primaryVariableTargetColumn`
- `columnSeparator`
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

- `Longitude [degrees_east]`
- `Latitude [degrees_north]`

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
  "value": "Pressure [dbar]"
}
```

### columnSeparator

Defines the source file separator.

Examples:

- tab (`\t`)
- comma
- semicolon

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

`timeZone` may be used when the ODV timestamp is assembled from separate
source columns. Prefer `UTC` or an IANA time zone name.

---

## Column Mapping Model

Column mappings are represented through:

`Dataset.variableMeasured[]`

Each item is a schema.org `PropertyValue` and represents one source column.

Required:

- `PropertyValue.name`

The value MUST match the exact source column header.

For regular data and metadata columns, the ODV output column is defined by:

`additionalProperty(name="targetColumn")`

Example:

```json
{
  "@type": "PropertyValue",
  "name": "Lon",
  "description": "Longitude of the sampling location.",
  "unitText": "degrees_east",
  "propertyID": "https://vocab.nerc.ac.uk/collection/P01/current/ALONZZ01/",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "targetColumn",
      "value": "Longitude [degrees_east]"
    }
  ]
}
```

---

## Native schema.org Column Metadata

Columns SHOULD use native schema.org properties where available.

### description

Use `PropertyValue.description` for human-readable variable definitions,
comments, explanations, or method notes.

### unitText

Use `PropertyValue.unitText` for optional unit information from the
source dataset/provider.

`unitText` is informational metadata. The ODV converter MUST NOT derive
ODV output column names from `unitText`.

The authoritative ODV variable name and unit label are defined by
`targetColumn`.

Example:

- `targetColumn`: `Temperature [degC]`
- `unitText`: `Original source unit is degrees Celsius.`

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

It defines the ODV output column name.

The following target columns are mandatory:

- `Longitude [degrees_east]`
- `Latitude [degrees_north]`

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
`Longitude [degrees_east]`, `Latitude [degrees_north]`,
`yyyy-mm-ddThh:mm:ss.sss`, and `Type`, converters may infer `role = meta`
and the appropriate datatype. Therefore `role` and `dataType` do not need to
be repeated for these known ODV columns.

### relatedColumn

Required for auxiliary columns with `role = quality` or `role = standardDeviation`.

`relatedColumn` contains the final ODV target column name of the measured
variable to which the auxiliary column belongs.

It MUST refer to the `targetColumn` value, not to the original source column
name.

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
      "value": "Temperature [degC]"
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
- `Longitude [degrees_east]`
- `Latitude [degrees_north]`
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
  "unitText": "unitless",
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
  "unitText": "degrees_Celsius",
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
3. Apply dataset parsing hints:
   - `columnSeparator`
   - `fillValue`
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
