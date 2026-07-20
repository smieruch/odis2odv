#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <QJsonArray>

//======================================================================
// Data Structures
//======================================================================

struct DatasetProperties
{
    QString dataField;
    QString dataType;
    QString primaryVariableTargetColumn;
    QString columnSeparator;
    QJsonValue fillValue;
};

//======================================================================
// Function Declarations
//======================================================================
QByteArray readLocalFile(const QString &source,
                         QTextStream& errorOutput);

QString getRequiredString(const QJsonObject& object,
                          const QString& key,
                          QTextStream& errorOutput);

QJsonObject getRequiredObject(const QJsonObject& object,
                              const QString& key,
                              QTextStream& errorOutput);
QJsonArray getRequiredArray(const QJsonObject& object,
                            const QString& key,
                            QTextStream& errorOutput);
void inspectVariableMeasured(const QJsonArray &variables,
                             QTextStream& out);
//======================================================================
// main
//======================================================================
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("odisodv");
    QCoreApplication::setApplicationVersion("0.1.0");

    //command line parser object
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
      
    if (command == "inspect") {
      //read the local file
      const QByteArray data = readLocalFile(source, out);      
      
      if (data.isEmpty()) {
	return 1;
      }

      out << "Inspecting source:\n";
      out << source << "\n";
      out << "Loaded " << data.size() << " bytes\n";

      //check if it is JSON
      QJsonParseError parseError;

      const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);

      if (parseError.error != QJsonParseError::NoError) {
        out << "Invalid JSON:\n";
        out << parseError.errorString() << "\n";
        out << "Offset: " << parseError.offset << "\n";
	return 1;
      }

      //check if it is an object
      if (!document.isObject()) {
        out << "JSON root is not an object.\n";
	return 1;
      }

      //get JSON root object, i.e. all
      const QJsonObject root = document.object();


      out << "Valid JSON\n";
      out << "Top-level JSON object\n";
      out << "Number of properties: " << root.size() << "\n";

      //get type and name
      const QString type = getRequiredString(root, "@type", out);

      if (type.isEmpty()) {
	return 1;
      }
      
      const QString name = getRequiredString(root, "name", out);
      
      if (name.isEmpty()) {
	return 1;
      }
      
      out << "@type: " << type << "\n";
      out << "Name: " << name << "\n";

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
      out << dataDownloadDocument.toJson(QJsonDocument::Indented) << "\n";

      
      const QString contentUrl = getRequiredString(dataDownload, "contentUrl", out);

      if (contentUrl.isEmpty()) {
	return 1;
      }

      out << "Content Url: " << contentUrl << "\n";
      
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
// getRequiredString()
//
// Reads a mandatory string property from a JSON object.
//
// The function verifies that the property
//   - exists,
//   - is a JSON string, and
//   - is not empty.
//
// Responsibility:
//     JSON object -> validated QString
//----------------------------------------------------------------------
QString getRequiredString(const QJsonObject& object,
                          const QString& key,
                          QTextStream& errorOutput)
{
    const QJsonValue value = object.value(key);

    if (value.isUndefined()) {
        errorOutput << "Missing required property: " << key << "\n";
        return {};
    }

    if (!value.isString()) {
        errorOutput << "Property is not a string: " << key << "\n";
        return {};
    }

    const QString text = value.toString();

    if (text.isEmpty()) {
        errorOutput << "Property is an empty string: " << key << "\n";
        return {};
    }

    return text;
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
      
      return {};
    }

    if (!value.isObject()) {
      errorOutput << "Property is not a JSON object: "
		  << key
		  << "\n";
      
      return {};
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
        return {};
    }

    if (!value.isArray()) {
        errorOutput << "Property is not a JSON array: "
                    << key
                    << "\n";
        return {};
    }

    return value.toArray();
}


//----------------------------------------------------------------------
// inspectVariableMeasured()
//
//----------------------------------------------------------------------

void inspectVariableMeasured(const QJsonArray& variables, QTextStream& out)
{
  for (const QJsonValue& value : variables) {
    if (!value.isObject()) {
      out << "Property is not an object: " << "\n";
    }
  }
  
  
  
  
}
