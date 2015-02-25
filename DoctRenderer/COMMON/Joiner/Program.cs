﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Joiner
{
    class Program
    {
        static void Main(string[] args)
        {
            string strApplication = Directory.GetCurrentDirectory();
            strApplication = Path.GetDirectoryName(strApplication);
            strApplication = Path.GetDirectoryName(strApplication);
            strApplication = Path.GetDirectoryName(strApplication);
            strApplication = Path.GetDirectoryName(strApplication);
            strApplication = Path.GetDirectoryName(strApplication);
            strApplication = Path.GetDirectoryName(strApplication);

            string strRoot = strApplication + "\\OfficeWeb\\";
            List<string> files = new List<string>();

            string[] arrFilesConfig = {
                "Common/Build/License.js",
                "Common/browser.js",
                "Common/docscoapisettings.js",
                "Common/docscoapicommon.js",
                "Common/docscoapi.js",
                "Common/apiCommon.js",
                "Common/spellcheckapisettings.js",
                "Common/spellcheckapi.js",
                "Common/wordcopypaste.js",
                "Common/spellCheckLanguage.js",
                "Common/spellCheckLanguagesAll.js",
                "Common/downloaderfiles.js",
                "Common/commonDefines.js",
                "Common/editorscommon.js",
                "Common/Shapes/Serialize.js",
                "Common/Shapes/SerializeWriter.js",
                "Common/SerializeCommonWordExcel.js",
				"Common/SerializeChart.js",
                "Common/Drawings/Format/Constants.js",
                "Common/Drawings/Format/Format.js",
                "Common/trackFile.js",
                "Common/FontsFreeType/font_engine.js",
                "Common/FontsFreeType/FontFile.js",
                "Common/FontsFreeType/FontManager.js",
                "Common/FontsFreeType/font_map.js",
                "Word/Drawing/Externals.js",
                "Word/Drawing/GlobalLoaders.js",
                "Word/Drawing/translations.js",



                "Common/Charts/charts.js",
                "Common/Charts/ChartsDrawer.js",
                "Common/Charts/DrawingArea.js",
                "Common/Charts/DrawingObjects.js",
                "Common/NumFormat.js",


                "Common/Drawings/TrackObjects/AdjustmentTracks.js",
                "Common/Drawings/TrackObjects/MoveTracks.js",
                "Common/Drawings/TrackObjects/NewShapeTracks.js",
                "Common/Drawings/TrackObjects/PolyLine.js",
                "Common/Drawings/TrackObjects/ResizeTracks.js",
                "Common/Drawings/TrackObjects/RotateTracks.js",
                "Common/Drawings/TrackObjects/Spline.js",

                "Common/Drawings/ArcTo.js",
                "Common/Drawings/ColorArray.js",
                "Common/Drawings/CommonController.js",
                "Common/Drawings/DrawingObjectsHandlers.js",
                "Common/Drawings/Hit.js",
                "Common/Drawings/Joined.js",
                "Common/Drawings/Math.js",


                "Common/Drawings/Format/Shape.js",
                "Common/Drawings/Format/Image.js",
                "Common/Drawings/Format/GroupShape.js",
                "Common/Drawings/Format/ChartSpace.js",
                "Common/Drawings/Format/ChartFormat.js",
                "Common/Drawings/Format/Constants.js",
                "Common/Drawings/Format/CreateGeometry.js",
                "Common/Drawings/Format/Format.js",
                "Common/Drawings/Format/Geometry.js",
                "Common/Drawings/Format/Path.js",
                "Common/Drawings/Format/TextBody.js",

                "Word/Editor/GraphicObjects/Format/ShapePrototype.js",
                "Word/Editor/GraphicObjects/Format/ImagePrototype.js",
                "Word/Editor/GraphicObjects/Format/GroupPrototype.js",
                "Word/Editor/GraphicObjects/Format/ChartSpacePrototype.js",
                "Word/Editor/GraphicObjects/DrawingStates.js",
                "Word/Editor/GraphicObjects/GraphicObjects.js",
                "Word/Editor/GraphicObjects/GraphicPage.js",
                "Word/Editor/GraphicObjects/WrapManager.js",

                "Word/Editor/CollaborativeEditing.js",
                "Word/Editor/Comments.js",
                "Word/Editor/History.js",
                "Word/Editor/Styles.js",                
                "Word/Editor/FlowObjects.js",
                "Word/Editor/ParagraphContentBase.js",
                "Word/Editor/ParagraphContent.js",
                "Word/Editor/Hyperlink.js",
                "Word/Editor/Run.js",
                "Word/Editor/Math.js",
                "Word/Editor/Paragraph.js",
                "Word/Editor/Paragraph_Recalculate.js",
                "Word/Editor/Sections.js",
                "Word/Editor/Numbering.js",
                "Word/Editor/HeaderFooter.js",
                "Word/Editor/Document.js",
                "Word/Editor/Common.js",
                "Word/Editor/DocumentContent.js",
                "Word/Editor/Table.js",
                "Word/Editor/Serialize2.js",
                "Word/Editor/Search.js",
                "Word/Editor/FontClassification.js",
                "Word/Editor/Spelling.js",
                "Word/Drawing/Graphics.js",
                "Word/Drawing/Overlay.js",
                "Word/Drawing/HatchPattern.js",
                "Word/Drawing/ShapeDrawer.js",
                "Word/Drawing/Metafile.js",
                "Word/Drawing/DrawingDocument.js",
                "Word/Drawing/GraphicsEvents.js",
                "Common/Scrolls/iscroll.js",
                "Word/Drawing/WorkEvents.js",
                "Word/Drawing/Controls.js",
                "Word/Drawing/Rulers.js",
                "Word/Drawing/HtmlPage.js",
                "Word/Drawing/documentrenderer.js",
                "Common/scroll.js",
                "Word/Editor/SerializeCommon.js",
                "Word/document/empty.js",
				"Word/Math/mathTypes.js",
				"Word/Math/mathText.js",
				"Word/Math/mathContent.js",
				"Word/Math/base.js",
				"Word/Math/fraction.js",
				"Word/Math/degree.js",
				"Word/Math/matrix.js",
				"Word/Math/limit.js",
				"Word/Math/nary.js",
				"Word/Math/radical.js",
				"Word/Math/operators.js",
				"Word/Math/accent.js",
				"Word/Math/borderBox.js",
				
				"Excel/utils/utils.js",
				"Excel/model/CellComment.js",
				"Excel/model/Serialize.js",
				"Excel/model/WorkbookElems.js",
				"Excel/model/Workbook.js",
				"Excel/model/CellInfo.js",
				"Excel/model/AdvancedOptions.js",

                "Common/Shapes/EditorSettings.js",
                "Common/commonDefines.js",
                "Word/apiDefines.js",
                "Word/apiCommon.js",
                "Word/api.js",
            };

            if (true)
            {
                files.Clear();
                for (int i = 0; i < arrFilesConfig.Length; ++i)
                {
                    files.Add(arrFilesConfig[i]);
                }
            }

            StringBuilder oBuilder = new StringBuilder();

            for (int i = 0; i < files.Count; i++)
            {
                StreamReader oReader = new StreamReader(strRoot + files[i]);
                oBuilder.Append(oReader.ReadToEnd());

                oBuilder.Append("\n\n");
            }

            string strDestPath = strApplication + "\\OfficeWeb\\Word\\sdk-all.js";
            StreamWriter oWriter = new StreamWriter(strDestPath, false, Encoding.UTF8);
            
            oWriter.Write(oBuilder.ToString());
            oWriter.Close();
        }
    }
}
