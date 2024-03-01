<?xml version="1.0" encoding="UTF-8"?>
<!--================================================================================-->
<!-- Copyright 2003 - Delcam plc Birmingham 2003                                    -->
<!--================================================================================-->
<!-- History                                                                        -->
<!-- Who When         What                                                            -->
<!-- === ========  =============================================================== -->
<!-- FHA 09/12/2003  Written.                                                        -->
<!-- FHA 12/12/2003  Fix the bug of the cone that does not show offset                                                       -->
<!-- 					 Work with the fixed version of the catalogue -->
<!--================================================================================-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>
	
	<xsl:variable name="digits" select="'0.000'"/>
	
	
	
	<!-- The main template match the root and write the skeleton of the document -->
	<xsl:template match="/">
	    <div>
				<xsl:apply-templates select="Catalogue/ProbeParts"/>
		  </div>
	</xsl:template>
	
	
	
	
	
	<!-- The probe parts template apply a specifique template for each child -->
	<xsl:template match="ProbeParts">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<h1>Probe parts catalogue</h1>
		<font style="CURSOR: hand; COLOR: blue">Quick Links:</font>
		<br/>
		
		<li>
			<a href="#Connectors">Connectors</a>
		</li>
		<li>
			<a href="#Extensions">Extensions</a>
		</li>
		<li>
			<a href="#Modules">Modules</a>
		</li>
		<li>
			<a href="#ProbeBody">Probe Body</a>
		</li>
		<li>
			<a href="#Stylus">Stylus</a>
		</li><br/><br/>
		<span>
			<INPUT onclick="ShowAll()" type="button" value="Show all" style="CURSOR: hand"/>
			<INPUT onclick="HideAll()" type="button" value="Hide all" style="CURSOR: hand"/>
		</span>

		<a name="Connectors"/>

		<h3>Connectors</h3>

    <blockquote>
		  <div style="TEXT-INDENT: 1em">
			  <xsl:apply-templates select="Connector"/>
		  </div>
    </blockquote>


		<a name="Extensions"/>

		<h3>Extensions</h3>

    <blockquote>
		  <div style="TEXT-INDENT: 1em">
			  <xsl:apply-templates select="Extension"/>
		  </div>
    </blockquote>


		<a name="Modules"/>
		<h3>Modules</h3>
    <blockquote>
		  <div style="TEXT-INDENT: 1em">
			  <xsl:apply-templates select="Module"/>
		  </div>
    </blockquote>

		<a name="ProbeBody"/>

		<h3>Probe Body</h3>
    <blockquote>
		  <div style="TEXT-INDENT: 1em">
			  <xsl:apply-templates select="ProbeBody"/>
		  </div>
    </blockquote>
		<a name="Stylus"/>
		<h3>Stylus</h3>
    <blockquote>
		<div style="TEXT-INDENT: 1em">
			<xsl:apply-templates select="Stylus"/>
		</div>
    </blockquote>
	</xsl:template>
	
	
	
	
	
	<!-- Display the name and the manufacturer -->
	<xsl:template name="Title">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<strong style="TEXT-ALIGN: center">
			<xsl:value-of select="./Manufacturer/text()"/>
			<xsl:text>  </xsl:text>
			<xsl:value-of select="@name"/>
		</strong>
		<xsl:text> </xsl:text>
	</xsl:template>
	
	
	
	
	
	
	
	<!--Connector  -->
	<xsl:template match="Connector">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<!-- Write the manufacturer and the name of the connector -->
		<div>
			<table nowrap="true" >
				<tr>
					<td>
						<font class="toggle" onclick="ToggleDisplay(this)">[+]</font>
						<xsl:call-template name="Title"/>
					</td>
				</tr>
				<tr  type="expandable" style="display:none">
					<td>
						<p>
							<font class="NameStyle">
								<xsl:text>From mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@from_mounting"/>
								</font>
								<xsl:text>  Price:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@price"/>
								</font>
							</font>
							<br/>
							<br/>
							<xsl:apply-templates select="Geometry"/>
							<br/>
							<xsl:apply-templates select="Explosions"/>
						</p>
					</td>
				</tr>
			</table>
		</div>
	</xsl:template>
	
	
	
	
	
	
	
	<!-- Extension -->
	<xsl:template match="Extension">
		<!-- FHA 09/12/2003  Written.-->
		<div>
			<table nowrap="true" >
				<tr>
					<td>
						<font class="toggle" onclick="ToggleDisplay(this)">[+]</font>
						<xsl:call-template name="Title"/>
					</td>
				</tr>
				<tr  style="display:none" >
					<td>
						<p>
							<font class="NameStyle">
								<xsl:text>From mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@from_mounting"/>
								</font>
								<xsl:text>  To mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@to_mounting"/>
								</font>
								<xsl:text>  Length:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@length"/>
								</font>
								<xsl:text>  Price:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@price"/>
								</font>
							</font>
							<table>
							<tr>
								<xsl:call-template name="CheckLengthDisplay">
									<xsl:with-param name="length" select="@length"/>
									<xsl:with-param name="entityList" select="./Geometry/*"/>
								</xsl:call-template>
							</tr>
							</table>
							<br/>
							<br/>
							<xsl:apply-templates select="Geometry"/>
						</p>
					</td>
				</tr>
			</table>
		</div>
	</xsl:template>
	
	
	
	
	
	
	
	
	
	<!-- Module -->
	<xsl:template match="Module">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<div>
		<table nowrap="true" >
				<tr>
					<td>
						<font class="toggle" onclick="ToggleDisplay(this)">[+]</font>
						<xsl:call-template name="Title"/>
					</td>
				</tr>
				<tr  style="display:none" >
					<td>
						<p>
							<font class="NameStyle">
								<xsl:text>From mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@from_mounting"/>
								</font>
								<xsl:text>  To mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@to_mounting"/>
								</font>
								
								<xsl:text>  Length:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@length"/>
								</font>

								<xsl:text>  Docking height:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@docking_height"/>
								</font>
								<xsl:text>  Price:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@price"/>
								</font>
							</font>
							<table>
							<tr>
								<xsl:call-template name="CheckLengthDisplay">
									<xsl:with-param name="length" select="@length"/>
									<xsl:with-param name="entityList" select="./Geometry/*"/>
								</xsl:call-template>
							</tr>
							</table>
							<br/>
							<br/>
							<xsl:apply-templates select="Geometry"/>
						</p>
					</td>
				</tr>
			</table>
		</div>
	</xsl:template>
	
	
	
	<!-- ProbeBody -->
	<xsl:template match="ProbeBody">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<div>
			<table nowrap="true" >
				<tr>
					<td>
						<font class="toggle" onclick="ToggleDisplay(this)">[+]</font>
						<xsl:call-template name="Title"/>
					</td>
				</tr>
				<tr style="display:none" >
					<td>
						<p>
							<font class="NameStyle">
								<xsl:text>From mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@from_mounting"/>
								</font>
								<xsl:text>  To mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@to_mounting"/>
								</font>
								
								<xsl:text>  Length:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@length"/>
								</font>

								<xsl:text>  Docking height:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@docking_height"/>
								</font>
								<xsl:text>  Price:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@price"/>
								</font>
							</font>
							<table>
							<tr>
								<xsl:call-template name="CheckLengthDisplay">
									<xsl:with-param name="length" select="@length"/>
									<xsl:with-param name="entityList" select="./Geometry/*"/>
								</xsl:call-template>
							</tr>
							</table>
							<br/>
							<br/>
							<xsl:apply-templates select="Geometry"/>
						</p>
					</td>
				</tr>
			</table>
			</div>
	</xsl:template>
	
	
	
	<!-- Stylus -->
	<xsl:template match="Stylus">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<div>
			<table nowrap="true" >
				<tr>
					<td>
						<font class="toggle" onclick="ToggleDisplay(this)">[+]</font>
						<xsl:call-template name="Title"/>
					</td>
				</tr>
				<tr type="colapsable" style="display:none" >
					<td>
						<p>
							<font class="NameStyle">
								<xsl:text>From mounting:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@from_mounting"/>
								</font>
								<xsl:text>  Diameter:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@diameter"/>
								</font>
								
								<xsl:text>  Length:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@length"/>
								</font>

								<xsl:text>  Price:</xsl:text>
								<font class="ValueStyle">
									<xsl:value-of select="@price"/>
								</font>
							</font>
							<table>
							<tr>
								<xsl:call-template name="CheckLengthDisplay">
									<xsl:with-param name="length" select="@length"/>
									<xsl:with-param name="entityList" select="./Geometry/*"/>
								</xsl:call-template>
							</tr>
							</table>
							<br/>
							<br/>
							<xsl:apply-templates select="Geometry"/>
						</p>
					</td>
				</tr>
			</table>
			</div>
	</xsl:template>
	
	
	
	
	<!-- Geometry -->
	<xsl:template match="Geometry">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<div>
			<strong>Geometry</strong>
			<table style="display:block" class="GeometryTable" cellSpacing="1" cellPadding="1" border="1" >
				<tr>
					<td/>
					<td class="GeomtryEntity">height</td>
					<td class="GeomtryEntity">offset</td>
					<td class="GeomtryEntity">ref. len.</td>
					<td class="GeomtryEntity">diameter</td>
					<td class="GeomtryEntity">diam1</td>
					<td class="GeomtryEntity">diam2</td>
					<td class="GeomtryEntity">material</td>
					<td class="GeomtryEntity">color</td>
				</tr>
				<xsl:for-each select="./*">
					<xsl:call-template name="Entity">
						<xsl:with-param name="item" select="current()"/>
					</xsl:call-template>
				</xsl:for-each>
			</table>
		</div>
	</xsl:template>
	<xsl:template name="CheckLengthDisplay">
		<xsl:param name="length"/>
		<xsl:param name="entityList"/>
		<xsl:variable name="length_checking">
			<xsl:call-template name="CheckLength">
				<xsl:with-param name="list" select="$entityList"/>
			</xsl:call-template>
		</xsl:variable>
		<td>
			<xsl:attribute name="bgcolor"><xsl:choose><xsl:when test="number($length_checking) != $length"><xsl:value-of select="'red'"/></xsl:when><xsl:otherwise><xsl:value-of select="'lime'"/></xsl:otherwise></xsl:choose></xsl:attribute>
			<xsl:text>	checking:</xsl:text>
			<xsl:value-of select="number($length_checking)"/>
		</td>
	</xsl:template>
	<xsl:template name="Cylinder">
		<!-- FHA 09/12/2003  Written. -->
		<xsl:param name="cylinder"/>
		<tr>
			<td class="GeomtryEntity">Cylinder</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cylinder/@height,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cylinder/@offset,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cylinder/@reference_length,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cylinder/@diameter,$digits)"/>
			</td>
			<td class="GeomtryAttributes"><xsl:text>N/A</xsl:text></td>
			<td class="GeomtryAttributes"><xsl:text>N/A</xsl:text></td>
			<td class="GeomtryAttributesText">
				<xsl:value-of select="$cylinder/@material"/>
			</td>
			<td>
				<xsl:attribute name="bgcolor"><xsl:value-of select="$cylinder/@color"/></xsl:attribute>
			</td>
		</tr>
	</xsl:template>
	
	
	<xsl:template name="Cone">
		<!-- FHA 09/12/2003  Written. -->
		<xsl:param name="cone"/>
		<tr>
			<td class="GeomtryEntity">Cone</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cone/@height,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cone/@offset,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cone/@reference_length,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:text>N/A</xsl:text>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cone/@diameter1,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($cone/@diameter2,$digits)"/>
			</td>
			<td class="GeomtryAttributesText">
				<xsl:value-of select="$cone/@material"/>
			</td>
			<td>
				<xsl:attribute name="bgcolor"><xsl:value-of select="$cone/@color"/></xsl:attribute>
			</td>
		</tr>
	</xsl:template>
	
	
	
	<xsl:template name="Sphere">
		<!-- FHA 09/12/2003  Written. -->
		<xsl:param name="sphere"/>
		<tr>
			<td class="GeomtryEntity">Sphere</td>
			<td  class="GeomtryAttributes">
			  <xsl:text>N/A</xsl:text>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($sphere/@offset,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($sphere/@reference_length,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
				<xsl:value-of select="format-number($sphere/@diameter,$digits)"/>
			</td>
			<td class="GeomtryAttributes">
    			<xsl:text>N/A</xsl:text>
			</td>
			<td class="GeomtryAttributes">
    			<xsl:text>N/A</xsl:text>
			</td>
			<td class="GeomtryAttributesText">
				<xsl:value-of select="$sphere/@material"/>
			</td>
			<td>
				<xsl:attribute name="bgcolor"><xsl:value-of select="$sphere/@color"/></xsl:attribute>
			</td>
		</tr>
	</xsl:template>
	
	
	
	<!-- Do the sum checking the list is the all the entity of the node geometry. the for each of them -->
   <!-- I call sumLength witch is going to sum ref length and offset then i sum that with the sum of the rest -->

	<xsl:template name="CheckLength">
		<xsl:param name="list"/>
		<xsl:choose>
			<xsl:when test="count($list) = 0"/>
			<xsl:when test="count($list) = 1">
				<xsl:call-template name="SumLength">
					<xsl:with-param name="item" select="$list[1]"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="x">
					<xsl:call-template name="SumLength">
						<xsl:with-param name="item" select="$list[1]"/>
					</xsl:call-template>
				</xsl:variable>
				<xsl:variable name="y">
					<xsl:call-template name="CheckLength">
						<xsl:with-param name="list" select="$list[1!=position()]"/>
					</xsl:call-template>
				</xsl:variable>
				<xsl:value-of select="number($x) + number($y)"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	
	
	<xsl:template name="SumLength">
		<xsl:param name="item"/>
		<xsl:value-of select="$item/@offset + $item/@reference_length"/>
		<!--
		<xsl:variable name="offset" select="$item/@offset"/>
		<xsl:choose>
			<xsl:when test="name($item) = 'Sphere'">
				<xsl:value-of select="0"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$offset + $item/@reference_length"/>			
			</xsl:otherwise>
		</xsl:choose>
		-->
	</xsl:template>
	<!-- add a ligne to the current table. This line represent the entity and the attributes -->
	<xsl:template name="Entity">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<xsl:param name="item"/>
		<xsl:choose>
			<xsl:when test="name() = 'Cylinder'">
				<xsl:call-template name="Cylinder">
					<xsl:with-param name="cylinder" select="$item"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="name() = 'Cone'">
				<xsl:call-template name="Cone">
					<xsl:with-param name="cone" select="$item"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="name() = 'Sphere'">
				<xsl:call-template name="Sphere">
					<xsl:with-param name="sphere" select="$item"/>
				</xsl:call-template>
			</xsl:when>
		</xsl:choose>
	</xsl:template>
	
	<!-- a recursive template to diaplay the attribute -->
	<xsl:template name="DisplayAttributes">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<xsl:param name="attributeList"/>
		<xsl:if test="count($attributeList)>0">
			<td class="GeomtryAttributes">
				<xsl:value-of select="local-name($attributeList[1])"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="$attributeList[1]"/>
			</td>
			<xsl:call-template name="DisplayAttributes">
				<xsl:with-param name="attributeList" select="$attributeList[1!=position()]"/>
			</xsl:call-template>
		</xsl:if>
	</xsl:template>
	
	
	<!--Explosions -->
	<xsl:template match="Explosions">
		<!-- FHA 09/12/2003  Written.                                                        -->
		<div>
			<strong>Explosions</strong>
			<table style="display:block" class="GeometryTable" cellSpacing="1" cellPadding="1" border="1">
				<tr>
					<xsl:apply-templates select="Explosion"/>
				</tr>
			</table>
		</div>
	</xsl:template>
	
	
	<!-- Explosion -->
	<xsl:template match="Explosion">
		<td nowrap="true">
			<h5>
				<xsl:text>Explosion </xsl:text>
				<xsl:value-of select="@name"/>
				<xsl:text> ID: </xsl:text>
				<xsl:value-of select="@id"/>
			</h5>
			<table class="ExplosionTable">
				<tr>
					<td width="25%"/>
					<td class="GeomtryEntity" width="25%">I</td>
					<td class="GeomtryEntity" width="25%">J</td>
					<td class="GeomtryEntity" width="25%">K</td>
				</tr>
				<tr>
					<td class="GeomtryEntity">Direction</td>
					<td class="GeomtryAttributes">
						<xsl:value-of select="./Direction/@I"/>
					</td>
					<td class="GeomtryAttributes">
						<xsl:value-of select="./Direction/@J"/>
					</td>
					<td class="GeomtryAttributes">
						<xsl:value-of select="./Direction/@K"/>
					</td>
				</tr>
				<tr>
					<td width="25%"/>
					<td class="GeomtryEntity" width="25%">X</td>
					<td class="GeomtryEntity" width="25%">Y</td>
					<td class="GeomtryEntity" width="25%">Z</td>
				</tr>
				<tr>
					<td class="GeomtryEntity" width="25%">Point</td>
					<td class="GeomtryAttributes">
						<xsl:value-of select="./Point/@X"/>
					</td>
					<td class="GeomtryAttributes">
						<xsl:value-of select="./Point/@Y"/>
					</td>
					<td class="GeomtryAttributes">
						<xsl:value-of select="./Point/@Z"/>
					</td>
				</tr>
			</table>
		</td>
	</xsl:template>
</xsl:stylesheet>
