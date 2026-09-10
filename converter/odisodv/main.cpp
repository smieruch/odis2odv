#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <array>
#include <utility>

//======================================================================
// Helper Types
//======================================================================

enum class EmptyPolicy { NotAllow, Allow };
enum class MissingPolicy { NotAllow, Allow };

//======================================================================
// Structs
//======================================================================

struct VariableDefinition {
  QString sourceColumn;
  QString description;
  QString unitText;
  QString propertyID;
  
  QString targetColumn;
  QString unit;
  QString unitID;
  QString dataType;
  QString role;
  QString relatedColumn;
  QString qualityFlagScheme;
};

//======================================================================
// Function Declarations
//======================================================================
bool parseJsonDocument(const QByteArray &data, QJsonObject &root,
                       QTextStream &errorOutput);

QByteArray readLocalFile(const QString &source,
                         QTextStream& errorOutput);

bool getString(const QJsonObject &object, const QString &key, QString& result,
                  QTextStream &errorOutput,
                  EmptyPolicy emptyPolicy = EmptyPolicy::NotAllow,
		  MissingPolicy missingPolicy = MissingPolicy::NotAllow);

QJsonObject getRequiredObject(const QJsonObject& object,
                              const QString& key,
                              QTextStream& errorOutput);

QJsonArray getRequiredArray(const QJsonObject& object,
                            const QString& key,
                            QTextStream& errorOutput);

bool parseDatasetProperties(const QJsonArray& properties,
			    QMap<QString, QString>& result,
			    QTextStream& errorOutput);

bool parseVariables(const QJsonArray& variables,
                    QList<VariableDefinition>& result,
                    QTextStream& errorOutput);

//======================================================================
// main
//======================================================================
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("odisodv");
    QCoreApplication::setApplicationVersion("0.1.0");

    //----------------------------------------------------------------------
    // command line parser object
    //----------------------------------------------------------------------
    QCommandLineParser parser;
    parser.setApplicationDescription("ODIS/ODV converter");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("command", "Command: inspect, convert, validate");
    parser.addPositionalArgument("source", "Input JSON-LD, ODV file, or URL");

    //read the command line arguments
    parser.process(app);
    
    //output object
    QTextStream out(stdout);
    
    const QStringList args = parser.positionalArguments();
    
    if (args.isEmpty()) {
        out << "No command given.\n\n";
        parser.showHelp(1);
    }

    if (args.size() < 2) {
      out << "Missing source.\n\n";
      parser.showHelp(1);
    }

    const QString command = args.at(0);
    const QString source = args.at(1);
    //----------------------------------------------------------------------


    
    //----------------------------------------------------------------------
    // now we start processing, read file
    //----------------------------------------------------------------------
    const QByteArray data = readLocalFile(source, out);      
      
    if (data.isEmpty()) {
      return 1;
    }
    
    out << "Read local file:\n";
    out << source << "\n";
    out << "Loaded " << data.size() << " bytes\n";



    //----------------------------------------------------------------------
    // first command inspect
    //----------------------------------------------------------------------
    if (command == "inspect") {

      QJsonObject root;

      //check if it json and extract the root object
      if (!parseJsonDocument(data, root, out)) {
        return 1;
      }

      out << "Valid JSON\n";
      out << "Top-level JSON object\n";
      out << "Number of properties: " << root.size() << "\n";
      

      //get type and name and schemaVersion
      QString type;
      if (!getString(root, "@type", type, out, EmptyPolicy::NotAllow)) {
	return 1;
      }


      QString name;
      if (!getString(root, "name", name, out, EmptyPolicy::NotAllow)){
	return 1;
      }

      QString schemaVersion;
      if (!getString(root, "schemaVersion", schemaVersion, out, EmptyPolicy::NotAllow)){
	return 1;
      }
      
      out << "@type: " << type << "\n";
      out << "Name: " << name << "\n";
      out << "schemaVersion: " << schemaVersion << "\n";
      //----------------------------------------------------------------------


      
      //get distribution array, it has the data download link
      const QJsonArray distribution = getRequiredArray(root, "distribution", out);

      if (distribution.isEmpty()) {
	out << "Property is an empty array: distribution\n";
	return 1;
      }

      const QJsonValue firstDistribution = distribution.at(0);

      if (!firstDistribution.isObject()) {
	out << "First distribution entry is not a JSON object\n";
      }

      const QJsonObject dataDownload = firstDistribution.toObject();

      // print out the dataDownload to check what's inside
      QJsonDocument dataDownloadDocument(dataDownload);
      out << "dataDownload:\n";
      // out << dataDownloadDocument.toJson(QJsonDocument::Indented) << "\n";

      QString contentUrl;
      if (!getString(dataDownload, "contentUrl", contentUrl, out)){
	return 1;
      }

      out << "Content Url: " << contentUrl << "\n";
      
      //get additionalProperty that contain the DatasetProperties
      //Q: what if a json contains another additionalProperty? Or must that be unique?
      const QJsonArray additionalProperty = getRequiredArray(root, "additionalProperty", out);

      if (additionalProperty.isEmpty()) {
	out << "Property is an empty array: additionalProperty\n";
	return 1;
      }

      //Map
      QMap<QString, QString> datasetProperties;

      if (!parseDatasetProperties(additionalProperty, datasetProperties,
				  out)) {
        return 1;
      }

      //get variableMeasured array
      const QJsonArray variableMeasured = getRequiredArray(root, "variableMeasured", out);

      if (variableMeasured.isEmpty()) {
	out << "Property is an empty array: variableMeasured\n";
	return 1;
      }

      QList<VariableDefinition> variables;

      if (!parseVariables(variableMeasured, variables, out)) {
	out << "Error in parseVariables\n";
         return 1;
      }

      // Output the parsed variables
      for (const auto& variable : variables) {
        out << "Variable: " << variable.sourceColumn << "\n"; //name in json
        // out << "  Description: " << variable.description << "\n";
        // out << "  Unit Text: " << variable.unitText << "\n";
        // out << "  Property ID: " << variable.propertyID << "\n";
        // out << "  Target Column: " << variable.targetColumn << "\n";
        // out << "  Unit: " << variable.unit << "\n";
	// out << "  Unit ID: " << variable.unitID << "\n";
        // out << "  Data Type: " << variable.dataType << "\n";
        // out << "  Role: " << variable.role << "\n";
        // out << "  Related Column: " << variable.relatedColumn << "\n";
        // out << "  Quality Flag Scheme: " << variable.qualityFlagScheme << "\n";
      }

      return 0;
    }
    else if (command == "convert") {
      out << "Command not yet implemented: " << command << "\n";
      return 1;
    }
    else if (command == "validate") {
      out << "Command not yet implemented: " << command << "\n";
      return 1;
    }
    else {
      out << "Unknown command: " << command << "\n\n";
      parser.showHelp(1);
    }



    
    return 0;
    
}



//======================================================================
// Helper Functions
//======================================================================


//----------------------------------------------------------------------
// parseJsonDocument()
//
// Parses a JSON document from raw bytes.
//
// The function verifies that
//   - the input contains valid JSON, and
//   - the JSON root is an object.
//
// Responsibility:
//     raw JSON bytes -> validated QJsonObject
//----------------------------------------------------------------------
bool parseJsonDocument(const QByteArray &data, QJsonObject &root,
                       QTextStream &errorOutput)
{
  //check if it is JSON
  QJsonParseError parseError;
  
  const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
  
  if (parseError.error != QJsonParseError::NoError) {
    errorOutput << "Invalid JSON:\n";
    errorOutput << parseError.errorString() << "\n";
    errorOutput << "Offset: " << parseError.offset << "\n";
    return false;
  }

  //check if it is an object
  if (!document.isObject()) {
    errorOutput << "JSON root is not an object.\n";
    return 1;
  }

  //get JSON root object, i.e. all
  root = document.object();


  return true;

}


//----------------------------------------------------------------------
// readLocalFile()
//
// Opens a local file, reads its complete contents into a QByteArray,
// and reports file I/O errors.
//
// Responsibility:
//     local file -> raw bytes
//----------------------------------------------------------------------
QByteArray readLocalFile(const QString& source, QTextStream& errorOutput)
{
  QFile file(source);
  
  if (!file.open(QIODevice::ReadOnly)) {
    errorOutput << "Could not open source:\n";
    errorOutput << source << "\n";
    errorOutput << "Reason: " << file.errorString() << "\n";
    return {};
  }
  
  return file.readAll();
}


//----------------------------------------------------------------------
// getString()
//
// Reads a string property.
//
// The function verifies that the property
//   - exists,
//   - is a JSON string, and
//   - is non-empty unless EmptyPolicy::Allow is specified.
//   - is non-missing unless MissingPolicy::Allow is specified.
//
// Responsibility:
//     JSON object -> validated QString
//----------------------------------------------------------------------
bool getString(const QJsonObject &object, const QString &key, QString& result,
                  QTextStream &errorOutput,
                  EmptyPolicy emptyPolicy = EmptyPolicy::NotAllow,
		  MissingPolicy missingPolicy = MissingPolicy::NotAllow)
{
    const QJsonValue value = object.value(key);

    if (value.isUndefined()) {
      if (missingPolicy == MissingPolicy::NotAllow){
        errorOutput << "Missing required property: " << key << "\n";
	return false;
      }
      result.clear();
      return true;
    }

    if (!value.isString()) {
        errorOutput << "Property is not a string: " << key << "\n";
	return false;
    }

    const QString text = value.toString();

    if (emptyPolicy == EmptyPolicy::NotAllow && text.isEmpty()) {
        errorOutput << "Property is an empty string: " << key << "\n";
	return false;
    }

    result = text;
    return true;
}


//----------------------------------------------------------------------
// getRequiredObject()
//
// Reads a mandatory object property from a JSON object.
//
// The function verifies that the property
//   - exists, and
//   - is a JSON object.
//
// Responsibility:
//     JSON object -> validated QJsonObject
//----------------------------------------------------------------------
QJsonObject getRequiredObject(const QJsonObject& object,
                              const QString& key,
                              QTextStream& errorOutput)
{
    const QJsonValue value = object.value(key);

    if (value.isUndefined()) {
      errorOutput << "Missing required property: "
		  << key
		  << "\n";
      
      return QJsonObject();
    }

    if (!value.isObject()) {
      errorOutput << "Property is not a JSON object: "
		  << key
		  << "\n";
      
      return QJsonObject();
    }
    
    return value.toObject();
}


//----------------------------------------------------------------------
// getRequiredArray()
//
// Reads a mandatory array property from a JSON object.
//
// The function verifies that the property
//   - exists, and
//   - is a JSON array.
//
// Responsibility:
//     JSON object -> validated QJsonArray
//----------------------------------------------------------------------

QJsonArray getRequiredArray(const QJsonObject& object,
                            const QString& key,
                            QTextStream& errorOutput)
{
    const QJsonValue value = object.value(key);

    if (value.isUndefined()) {
        errorOutput << "Missing required property: "
                    << key
                    << "\n";
        return QJsonArray();
    }

    if (!value.isArray()) {
        errorOutput << "Property is not a JSON array: "
                    << key
                    << "\n";
	return QJsonArray();
    }

    return value.toArray();
}




//======================================================================
// Parsing Functions
//======================================================================


//---------------------------------------------------------------------
// parseDatasetProperties()
//
// Inspects the dataset-level additionalProperty array.
//
// Responsibility:
//     additionalProperty array -> validated property names
//---------------------------------------------------------------------
bool parseDatasetProperties(const QJsonArray& properties,
			    QMap<QString, QString>& result,
                              QTextStream& errorOutput)
{
    for (const QJsonValue& value : properties) {
        if (!value.isObject()) {
            errorOutput << "Dataset additionalProperty entry is not a JSON object\n";
            return false;
        }

        const QJsonObject propertyObject = value.toObject();

        QString propertyName;
        if (!getString(propertyObject, "name", propertyName, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        QString propertyValue;
        if (!getString(propertyObject, "value", propertyValue, errorOutput, EmptyPolicy::Allow, MissingPolicy::Allow)) {
            return false;
        }

        result.insert(propertyName, propertyValue);
        errorOutput << "Dataset property: " << propertyName << " = " << propertyValue << "\n";
    }

    return true;
}

//---------------------------------------------------------------------
// parseVariables()
//
// Inspects the variableMeasured array.
//
// Responsibility:
//     variableMeasured array -> validated VariableDefinition
//---------------------------------------------------------------------
bool parseVariables(const QJsonArray& variables,
                    QList<VariableDefinition>& result,
                    QTextStream& errorOutput)
{
    for (const QJsonValue& value : variables) {
        if (!value.isObject()) {
            errorOutput << "Variable entry is not a JSON object\n";
            return false;
        }

        const QJsonObject variableObject = value.toObject();

        VariableDefinition variable;

        if (!getString(variableObject, "name", variable.name, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "description", variable.description, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "unitText", variable.unitText, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "propertyID", variable.propertyID, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "targetColumn", variable.targetColumn, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "unit", variable.unit, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "unitID", variable.unitID, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "dataType", variable.dataType, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "role", variable.role, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "relatedColumn", variable.relatedColumn, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        if (!getString(variableObject, "qualityFlagScheme", variable.qualityFlagScheme, errorOutput, EmptyPolicy::NotAllow, MissingPolicy::NotAllow)) {
            return false;
        }

        result.append(variable);
    }

    return true;
}
