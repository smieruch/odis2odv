# ODIS → ODV Generic Spreadsheet Profile

This repository defines an **ODIS-compatible, schema.org-based profile**
for the **automatic generation of ODV Generic Spreadsheets** from
provider-supplied tabular ocean data (CSV / TSV).

The goal is to allow ocean data providers to publish **simple data files
plus structured, web-native metadata**, from which valid ODV Generic
Spreadsheets can be generated **deterministically and reproducibly**.

------------------------------------------------------------------------

## Motivation

Many ocean data providers publish tabular data (CSV/TSV), but do not
natively produce ODV files.

ODV Generic Spreadsheets require:

-   strict column labels,
-   mandatory geographic metadata (longitude, latitude),
-   specific conventions for variables and metadata.

This project bridges that gap by using:

-   **schema.org / JSON-LD** for dataset discovery (ODIS-compatible),
-   **schema.org-native mapping patterns** to describe column mappings,
-   **JSON Schema** to validate mapping metadata before conversion.

No custom JSON-LD vocabularies or namespaces are required.

ODIS2ODV uses schema.org classes and properties as the carrier model.
ODV-specific conversion instructions are expressed as controlled values
inside schema.org `PropertyValue` objects.

------------------------------------------------------------------------

## Design Principles

-   **Schema.org-based**\
    ODIS2ODV uses schema.org classes and properties wherever possible.
    No custom JSON-LD namespaces are required. ODV-specific conversion
    metadata is expressed as controlled values inside schema.org
    `PropertyValue` objects.
-   **Input-centric mapping**\
    Mappings are defined from *source columns* to *ODV output columns*.
-   **Provider-friendly**\
    Metadata is readable, writable, and understandable by
    non-programmers.
-   **ODIS-compatible**\
    Discovery metadata and conversion metadata coexist in one JSON-LD
    document.
-   **Deterministic conversion**\
    Given the same input and metadata, the same ODV spreadsheet is
    produced.

------------------------------------------------------------------------

## Architecture

ODIS2ODV separates FAIR discovery metadata from deterministic conversion
instructions.

``` text
ODIS / schema.org discovery layer
│
├── Dataset
├── DataDownload
├── variableMeasured
├── description
├── unitText        (optional source/provider unit notes)
└── propertyID     (semantic identifier, e.g. NERC P01)
        │
        ↓
FAIR discovery


ODIS2ODV conversion profile
inside PropertyValue
│
├── DataField
│     ├── GeneralField
│     ├── Ocean
│     ├── Atmosphere
│     ├── Land
│     ├── IceSheet
│     ├── SeaIce
│     └── Sediment
│
├── DataType
│     ├── GeneralType
│     ├── Profiles
│     ├── TimeSeries
│     └── Trajectories
│
├── targetColumn
├── primaryVariableTargetColumn
├── columnSeparator
├── fillValue
│
├── role
│     ├── meta
│     ├── data
│     ├── quality
│     ├── error
│     └── timeComponent
│
├── relatedColumn
├── qualityFlagScheme
├── dateTimeComponent
└── timeZone
        │
        ↓
deterministic ODV conversion
```

The JSON Schema does not define a new vocabulary. It defines the allowed
controlled values and validation rules of the ODIS2ODV profile.

------------------------------------------------------------------------

## Repository Contents

├── examples\
│ ├── ocean-data-test-003.json\
│ └── ocean-data-test-003.txt\
├── profile\
│ └── odis-profile-odv-generic-spreadsheet.md\
├── schema\
│ └── odv-odis2odv.schema.json\
├── tools\
│ └── jsonValidate.py\
├── jsonValidate.bash\
├── requirements.txt\
└── README.md

------------------------------------------------------------------------

### `profile/`

Human-readable profile description (ODIS Book style) defining scope,
semantics, and processing rules.

### `schema/`

JSON Schema for validating schema.org JSON-LD mapping documents.

### `examples/`

Minimal, working JSON-LD examples that:

-   pass schema validation,
-   can drive an ODV Generic Spreadsheet generator.

### `hot/`

JSON-LD files for the Hawaii Ocean Time-Series data hosted at BCO-DMO.

------------------------------------------------------------------------

## Dataset-level ODV Metadata

ODV collection information is stored using schema.org
`additionalProperty`:

-   `DataField` → ODV domain. Supported values: `GeneralField`, `Ocean`,
    `Atmosphere`, `Land`, `IceSheet`, `SeaIce`, `Sediment`
-   `DataType` → ODV collection type. Supported values: `GeneralType`,
    `Profiles`, `TimeSeries`, `Trajectories`
-   `primaryVariableTargetColumn` → primary ODV variable
-   `columnSeparator` → source table separator
-   `fillValue` → missing-value representation
-   `timeZone` → optional time zone used when assembling timestamps from
    separate source columns

------------------------------------------------------------------------

## Core Concept: Column Mapping via `variableMeasured`

Column mappings are expressed using **schema.org-native patterns**:

-   `Dataset.variableMeasured[]`
-   each entry is a `PropertyValue`

### Mapping model

For each source column:

-   `PropertyValue.name`\
    → exact header of the **source column**
-   `additionalProperty(name="targetColumn")`\
    → header of the **ODV output column**

Example:

``` json
{
  "@type": "PropertyValue",
  "name": "Lon",
  "description": "Longitude of the sampling location.",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "targetColumn",
      "value": "Longitude [degrees_east]"
    }
  ]
}
```

------------------------------------------------------------------------

------------------------------------------------------------------------

## Required and optional ODV core columns

The following ODV target columns are mandatory for ODIS2ODV conversion:

-   `Longitude [degrees_east]`
-   `Latitude [degrees_north]`

The following ODV target columns are strongly recommended but optional:

-   `Cruise`
-   `Station`
-   `yyyy-mm-ddThh:mm:ss.sss` (directly mapped or assembled from
    `dateTimeComponent` columns, see below)

The following ODV metadata column is optional:

-   `Type`

For the known ODV metadata columns:

-   `Cruise`
-   `Station`
-   `Longitude [degrees_east]`
-   `Latitude [degrees_north]`
-   `yyyy-mm-ddThh:mm:ss.sss`
-   `Type`

converters infer the role and datatype. Therefore these properties do
not need to be explicitly provided in the JSON-LD.


For known ODV metadata columns, the JSON-LD only needs the `targetColumn`
mapping. The converter automatically infers `role` and `dataType`.

Custom metadata variables still require explicit `role = meta` and
`dataType`.

If present, `Type` is interpreted as metadata text.

Allowed values in the source data column are:

-   `C` = CTD measurement
-   `B` = bottle measurement

The converter validates these values during data conversion.

## Enriching an existing ODIS JSON-LD for ODV compatibility

Many data providers already publish an ODIS-compatible
`schema.org/Dataset` JSON-LD document. ODIS2ODV does **not** replace
this metadata.

Instead, the existing JSON-LD document is enriched with additional
schema.org information that allows deterministic conversion into an ODV
Generic Spreadsheet.

### 1. Keep the existing Dataset metadata

Existing discovery metadata remains unchanged, for example:

-   `name`
-   `description`
-   `identifier`
-   `license`
-   `spatialCoverage`
-   `temporalCoverage`
-   `distribution`

The data file should be referenced through:

``` json
{
  "@type": "DataDownload",
  "contentUrl": "https://example.org/data.csv",
  "encodingFormat": "text/csv"
}
```

### 2. Add ODV dataset-level information

ODV-specific collection information is added through
`Dataset.additionalProperty`:

``` json
{
  "@type": "PropertyValue",
  "name": "DataField",
  "value": "Ocean"
}
```

Common ODIS2ODV properties are:

-   `DataField` with supported values: `GeneralField`, `Ocean`,
    `Atmosphere`, `Land`, `IceSheet`, `SeaIce`, `Sediment`
-   `DataType` with supported values: `GeneralType`, `Profiles`,
    `TimeSeries`, `Trajectories`
-   `primaryVariableTargetColumn`
-   `columnSeparator`
-   `fillValue`
-   `timeZone` (optional, for timestamp assembly)

### 3. Add mappings for measured variables

Each source column is described using `Dataset.variableMeasured`.

Example:

``` json
{
  "@type": "PropertyValue",
  "name": "Temperature",
  "description": "Sea water temperature",
  "unitText": "Original provider unit information: degree Celsius.",
  "propertyID": "https://vocab.nerc.ac.uk/collection/P01/current/TEMPPR01/",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "targetColumn",
      "value": "Temperature [degC]"
    },
    {
      "@type": "PropertyValue",
      "name": "role",
      "value": "data"
    }
  ]
}
```

Rules:

-   `name` is always the exact source column name
-   `targetColumn` defines the ODV output column including the ODV unit
    label (for example `Temperature [degC]`)
-   `description` contains human-readable explanations
-   `unitText` is optional and may contain additional source/provider
    unit information. It is not used for ODV column generation.
-   `propertyID` SHOULD contain persistent semantic identifiers (for
    example NERC vocabulary URIs)

### 4. Describe auxiliary variables

Some columns do not become independent ODV variables but provide
additional information for another column. Timestamp component columns
are handled separately using `role = timeComponent`.

#### Quality flags

``` json
{
  "@type": "PropertyValue",
  "name": "Temperature_QF",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "role",
      "value": "quality"
    },
    {
      "@type": "PropertyValue",
      "name": "qualityFlagScheme",
      "value": "WOCEBOTTLE"
    },
    {
      "@type": "PropertyValue",
      "name": "relatedColumn",
      "value": "Temperature"
    }
  ]
}
```

#### Errors and standard deviations

``` json
{
  "@type": "PropertyValue",
  "name": "Temperature_sd",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "role",
      "value": "error"
    },
    {
      "@type": "PropertyValue",
      "name": "relatedColumn",
      "value": "Temperature"
    }
  ]
}
```

### 5. Validate the enriched JSON-LD

The enriched document should validate against the ODIS2ODV JSON Schema
before conversion.

The converter only uses the information contained in the JSON-LD and
does not rely on provider-specific APIs or conventions.

------------------------------------------------------------------------

------------------------------------------------------------------------

## Timestamp assembly from separate columns

A provider does not need to supply a single ISO 8601 timestamp column.

If the source table contains separate columns such as `Year`, `Month`,
`Day`, `Hour`, `Minute`, or `Second`, ODIS2ODV can describe how the
converter should assemble the optional ODV timestamp column:

`yyyy-mm-ddThh:mm:ss.sss`

Example:

``` json
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
    },
    {
      "@type": "PropertyValue",
      "name": "targetColumn",
      "value": "yyyy-mm-ddThh:mm:ss.sss"
    }
  ]
}
```

Allowed `dateTimeComponent` values are:

-   `year`
-   `month`
-   `day`
-   `hour`
-   `minute`
-   `second`
-   `millisecond`
-   `date`
-   `time`

For combined date or time components, the following formats are
supported:

-   `date`
    -   `yyyy-mm-dd`
    -   `yyyy-mm`
-   `time`
    -   `hh:mm:ss.sss`
    -   `hh:mm:ss`
    -   `hh:mm`

If needed, `Dataset.additionalProperty` may define a `timeZone`, for
example `UTC`.

------------------------------------------------------------------------

## Metadata validation vs data validation

The JSON Schema validates the JSON-LD mapping metadata.

It can validate values that are present in the JSON-LD document, for
example `DataField`, `DataType`, `role`, `qualityFlagScheme`,
`dateTimeComponent`, and `targetColumn`.

It cannot validate values that occur only in the source data table, for
example:

-   `Type` column values (`C` or `B`)
-   longitude and latitude numeric ranges
-   timestamp component formats
-   quality flag values

These checks are part of converter-level data validation.

## Validation Workflow

Before conversion, metadata files SHOULD be validated:

1.  **JSON syntax**
2.  **JSON Schema Draft 2020-12 validation**
3.  *(Optional)* JSON-LD expansion sanity check

Create a Python environment:

``` bash
python3 -m venv .venv
source .venv/bin/activate

pip install -r requirements.txt
```

Validate the example metadata:

``` bash
./jsonValidate.bash
```

or directly:

``` bash
python3 tools/jsonValidate.py \
  schema/odv-odis2odv.schema.json \
  examples/ocean-data-test-003.json
```

A successful validation prints:

``` text
Valid!
```

The schema validates the ODIS2ODV mapping description before conversion
into an ODV Generic Spreadsheet.

### Note on units

ODIS2ODV uses `targetColumn` as the authoritative ODV output definition.

Example:

``` json
{
  "name": "targetColumn",
  "value": "Temperature [degC]"
}
```

The schema.org property `unitText` is optional. It can be used to
preserve additional source/provider unit information, but it is not
required for conversion and is not used to construct ODV column names.

------------------------------------------------------------------------

## License

Documentation, specifications, schemas, and examples in this repository
are licensed under the **Creative Commons Attribution 4.0 International
License (CC BY 4.0)**.

Software code (if added) is licensed under the **MIT License**, unless
stated otherwise.
