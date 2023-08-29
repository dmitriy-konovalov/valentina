const fs = require("fs");
const path = require("path");
const convert = require("xml-js");

let translationsDir =
  "/Users/dos095-russ/Documents/src/valentina/share/translations";
var measurements_files = fs
  .readdirSync(translationsDir)
  .filter(fn => fn.startsWith("measurements_") && fn.endsWith("ru_RU.ts"));

for (let i = 0; i < measurements_files.length; i++) {
  const fileNameRu = measurements_files[i];
  const fileNameEn = fileNameRu.replace("_ru_RU", "");

  var xmlRu = fs.readFileSync(path.join(translationsDir, fileNameRu), "utf8");
  var options = { ignoreComment: true, alwaysChildren: true };
  var dataRu = convert.xml2js(xmlRu, options);

  var xmlEn = fs.readFileSync(path.join(translationsDir, fileNameEn), "utf8");

  var dataEn = convert.xml2js(xmlEn, options);

  console.log(fileNameRu);

  for (let j = 0; j < dataRu.elements[1].elements[0].elements.length; j++) {
    const elementRu = dataRu.elements[1].elements[0].elements[j];

    if (elementRu.name == "name") continue;
    // console.log(elementRu);
    // console.log(elementRu.elements);
    //var locationRuNode = elementRu.elements[0];
    var sourceRuNode = elementRu.elements.find(el => el.name == "source");

    // console.log(locationRuNode);
    // console.log(sourceRuNode);
    let sourceElementEn = null;
    for (let k = 0; k < dataEn.elements[1].elements[0].elements.length; k++) {
      const elementEn = dataEn.elements[1].elements[0].elements[k];
      if (elementEn.name == "name") continue;

      var sourceEnNode = elementEn.elements.find(el => el.name == "source");

      if (sourceEnNode.elements[0].text == sourceRuNode.elements[0].text) {
        sourceElementEn = elementEn;
      }
    }
    if (sourceElementEn == null) {
      console.warn("Not found: " + sourceRuNode.elements[0].text);
      continue;
    }

    let sourceLocationNodes = sourceElementEn.elements.filter(
      el => el.name == "location"
    );

    let elementIndex = 0;
    while (elementIndex < elementRu.elements.length) {
      if (elementRu.elements[elementIndex].name == "location") {
        elementRu.elements.splice(elementIndex, 1);
      } else {
        elementIndex++;
      }
    }
    elementRu.elements.splice(0, 0, ...sourceLocationNodes);
  }

  const xmlRuPatched = convert.js2xml(dataRu, {
    spaces: 4,
    textFn: function (val, elementName) {
      return val.replaceAll("'", "&apos;");
    },
  });
  fs.writeFileSync(path.join(translationsDir, fileNameRu), xmlRuPatched);
}
