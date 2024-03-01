<?xml version="1.0" encoding="UTF-8"?>
<!--================================================================================-->
<!-- Copyright 2003 - Delcam plc Birmingham 2003                                    -->
<!--================================================================================-->
<!-- History                                                                        -->
<!-- Who When         What                                                            -->
<!-- === ========  =============================================================== -->
<!-- FHA 09/12/2003  Written.                                                        -->
<!-- FHA 12/12/2003  Remove the copy of the code by using the xsl:import statment now all the code is in             -->
<!--                           ProbePartCatalogueForHtml.xslt-->
<!--================================================================================-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:import href="ProbePartCatalogueForHtml.xslt"/>
	<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>
	
	
	
	
	
	<xsl:variable name="digits" select="'0.000'"/>
	
	
	
	<!-- The main template match the root and write the skeleton of the document -->
	<xsl:template match="/">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<html>
			<head>
				<title>ProbePartCatalogue</title>
				<!-- link to the cascading style sheet that rule the aspect of the out put -->
				<link rel="stylesheet" type="text/css" href="ProbePartCatalogue.css"/>
				<script language="jscript"><![CDATA[
				
				function HideAll(){
					var list = document.getElementsByTagName("font");
					var node;
					var n = list.length - 1;
					
					var i;
					for (i = 0; i != n; i++) {
					
					  var node = list[i];
					  var type = node.getAttribute("type");
					  var text = node.innerText;
					  if(type=="expandable"){
					  	if( text == "Hide"){
					    		ToggleDisplay(node);
					    }
					  }					   
					}
				}
				
				function ShowAll(){
					var list = document.getElementsByTagName("font");
					var node;
					var n = list.length - 1;
					
					var i;
					for (i = 0; i != n; i++) {
					
					  var node = list[i];
					  var type = node.getAttribute("type");
					  var text = node.innerText;
					  if(type=="expandable"){
					  	if( text == "Show"){
					    		ToggleDisplay(node);
					    }
					  }					   
					}
				}

				
				
				function ToggleDisplay(oCurrent){
					var innerText = oCurrent.innerText;
					var td = oCurrent.parentElement;
					var tr = td.parentElement;
					var table = tr.parentElement;

					var node = table.lastChild;	
					if(innerText == "Show"){
						oCurrent.innerText = "Hide";
						if(node != null){
							node.style.display=  "block";
							td.style.textAlign = "center";
							table.style.backgroundColor = "skyblue";
							
						}

					} else{
						oCurrent.innerText = "Show";					
						if(node != null){
							node.style.display= "none";
							td.style.textAlign = "left";
							table.style.backgroundColor = "white";
						}
					}
				}
			]]></script>
			</head>
			<body>
				<!-- Apply the template for the ProbeParts  from the imported stylesheet-->
				<xsl:apply-templates select="Catalogue/ProbeParts"/>
			</body>
		</html>
	</xsl:template>
	
</xsl:stylesheet>
