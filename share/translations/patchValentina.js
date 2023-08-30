const fs = require("fs");
const path = require("path");
const convert = require("xml-js");

let translationsDir =
  "/Users/dos095-russ/Documents/src/valentina/share/translations";

const fileNameRu = "valentina_ru_RU_old.ts";
const fileNameEn = "valentina.ts";

var xmlRu = fs.readFileSync(path.join(translationsDir, fileNameRu), "utf8");
var options = { ignoreComment: true, alwaysChildren: true };
var dataRu = convert.xml2js(xmlRu, options);

var xmlEn = fs.readFileSync(path.join(translationsDir, fileNameEn), "utf8");

var dataEn = convert.xml2js(xmlEn, options);

let unlocalizedStrings = [];

for (
  let contextIndex = 0;
  contextIndex < dataEn.elements[1].elements.length;
  contextIndex++
) {
  const contextEn = dataEn.elements[1].elements[contextIndex];
  let contextName = contextEn.elements.find(el => el.name == "name").elements[0]
    .text;
  console.log(contextName);

  let contextRu = dataRu.elements[1].elements.find(
    el =>
      el.elements.find(el => el.name == "name").elements[0].text == contextName
  );
  //   if (!contextRu) {
  //     console.warn("Not found in ru_RU: " + contextName);
  //     continue;
  //   }
  console.group();
  for (
    let messageIndex = 0;
    messageIndex < contextEn.elements.length;
    messageIndex++
  ) {
    const messageEn = contextEn.elements[messageIndex];
    if (messageEn.name == "name") {
      continue;
    }
    console.group();
    let messageSourceEn = messageEn.elements.find(el => el.name == "source")
      .elements[0].text;

    console.log(messageSourceEn);

    if (!contextRu) {
      unlocalizedStrings.push({
        context: contextName,
        messageSource: messageSourceEn,
      });
    } else {
      let messageRu = contextRu.elements.find(
        el =>
          el.name == "message" &&
          el.elements.find(el1 => el1.name == "source").elements[0].text ==
            messageSourceEn
      );

      let translationNodeEn = messageEn.elements.find(
        el => el.name == "translation"
      );

      if (!messageRu) {
        unlocalizedStrings.push({
          context: contextName,
          messageSource: messageSourceEn,
        });
      } else {
        let translationNodeRu = messageRu.elements.find(
          el => el.name == "translation"
        );

        if (translationNodeRu.elements.length > 0) {
          var translationRu = translationNodeRu.elements[0];
          translationNodeEn.elements[0] = translationRu;
        }
      }
    }

    console.groupEnd();
  }

  console.groupEnd();
  //   const elementEn = dataEn.elements[1].elements[0].elements[j];

  //   if (elementRu.name == "name") continue;
  //   // console.log(elementRu);
  //   // console.log(elementRu.elements);
  //   //var locationRuNode = elementRu.elements[0];
  //   var sourceRuNode = elementRu.elements.find(el => el.name == "source");

  //   // console.log(locationRuNode);
  //   // console.log(sourceRuNode);
  //   let sourceElementEn = null;
  //   for (let k = 0; k < dataEn.elements[1].elements[0].elements.length; k++) {
  //     const elementEn = dataEn.elements[1].elements[0].elements[k];
  //     if (elementEn.name == "name") continue;

  //     var sourceEnNode = elementEn.elements.find(el => el.name == "source");

  //     if (sourceEnNode.elements[0].text == sourceRuNode.elements[0].text) {
  //       sourceElementEn = elementEn;
  //     }
  //   }
  //   if (sourceElementEn == null) {
  //     console.warn("Not found: " + sourceRuNode.elements[0].text);
  //     continue;
  //   }

  //   let sourceLocationNodes = sourceElementEn.elements.filter(
  //     el => el.name == "location"
  //   );

  //   let elementIndex = 0;
  //   while (elementIndex < elementRu.elements.length) {
  //     if (elementRu.elements[elementIndex].name == "location") {
  //       elementRu.elements.splice(elementIndex, 1);
  //     } else {
  //       elementIndex++;
  //     }
  //   }
  //   elementRu.elements.splice(0, 0, ...sourceLocationNodes);
}

const xmlRuPatched = convert.js2xml(dataEn, {
  spaces: 4,
  textFn: function (val, elementName) {
    return val.replaceAll("'", "&apos;");
  },
});
fs.writeFileSync(
  path.join(translationsDir, "valentina_ru_RU_new.ts"),
  xmlRuPatched
);

console.log(`Total ulocalized strings: ${unlocalizedStrings.length}`);
console.group();
console.log(JSON.stringify(unlocalizedStrings));
console.groupEnd();

// Not found in ru_RU: DRW_TextCodec
// Not found in ru_RU: DialogAskCollectStatistic
// Not found in ru_RU: InitPieceLabelLanguages
// Not found in ru_RU: Placeholder
// Not found in ru_RU: T
// Not found in ru_RU: TabPins
// Not found in ru_RU: Tool
// Not found in ru_RU: VArc
// Not found in ru_RU: VInteractiveTool
// Not found in ru_RU: VPPiece
// Not found in ru_RU: VPSheet
// Not found in ru_RU: VSingleLineOutlineChar
// Not found in ru_RU: VSvgFontReader
// Not found in ru_RU: VTapeSettings
// Not found in ru_RU: VTextGraphicsItem
// Not found in ru_RU: VToolCut
// Not found in ru_RU: VisToolAlongLine
// Not found in ru_RU: VisToolArc
// Not found in ru_RU: VisToolArcWithLength
// Not found in ru_RU: VisToolBisector
// Not found in ru_RU: VisToolCutArc
// Not found in ru_RU: VisToolCutSpline
// Not found in ru_RU: VisToolCutSplinePath
// Not found in ru_RU: VisToolEllipticalArc
// Not found in ru_RU: VisToolNormal
// Not found in ru_RU: VisToolPointFromCircleAndTangent
// Not found in ru_RU: VisToolPointOfContact
// Not found in ru_RU: VisToolPointOfIntersectionCircles
// Not found in ru_RU: VisToolShoulderPoint
