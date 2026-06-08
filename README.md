# ODIS → ODV Generic Spreadsheet Profile

This repository defines an **ODIS-compatible, schema.org-only profile**
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
schema.org layer
│
├── Dataset
├── DataDownload
├── variableMeasured
├── PropertyValue
├── description
├── unitText
└── propertyID
        ↑
        |
        FAIR / ODIS discovery

--------------------------------

ODIS2ODV profile layer
inside PropertyValue.name/value
│
├── targetColumn
├── role
├── relatedColumn
├── qualityFlagScheme
├── DataField
├── DataType
├── primaryVariableTargetColumn
├── columnSeparator
└── fillValue
        ↑
        |
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

-   `DataField` → ODV domain (for example `Ocean`)
-   `DataType` → ODV collection type (for example `Profiles`,
    `TimeSeries`)
-   `primaryVariableTargetColumn` → primary ODV variable
-   `columnSeparator` → source table separator
-   `fillValue` → missing-value representation

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

-   `DataField`
-   `DataType`
-   `primaryVariableTargetColumn`
-   `columnSeparator`
-   `fillValue`

### 3. Add mappings for measured variables

Each source column is described using `Dataset.variableMeasured`.

Example:

``` json
{
  "@type": "PropertyValue",
  "name": "Temperature",
  "description": "Sea water temperature",
  "unitText": "degree Celsius",
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
-   `targetColumn` defines the ODV output column
-   `description` contains human-readable explanations
-   `unitText` contains the original unit information
-   `propertyID` SHOULD contain persistent semantic identifiers (for
    example NERC vocabulary URIs)

### 4. Describe auxiliary variables

Some columns do not become independent ODV variables but provide
additional information for another column.

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

## License

Documentation, specifications, schemas, and examples in this repository
are licensed under the **Creative Commons Attribution 4.0 International
License (CC BY 4.0)**.

Software code (if added) is licensed under the **MIT License**, unless
stated otherwise.

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
preserve additional unit information from the source provider, but it is
not required for conversion and is not used to construct ODV column
names.
