<?xml version="1.0" encoding="UTF-8"?>
<!--================================================================================-->
<!-- Copyright 2003 - Delcam plc Birmingham 2003                                    -->
<!--================================================================================-->
<!-- History                                                                        -->
<!-- Who When         What                                                            -->
<!-- === ========  =============================================================== -->
<!-- FHA 12/12/2003  Written.                                                        -->
<!--================================================================================-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>
	
	<!-- Match the root -->
	<xsl:template match="/Catalogue">
		<xsl:copy>
			<!-- copy the assemblies node in deep -->
			<xsl:copy-of select="Assemblies"/>
			<!-- copy the ProbeParts node in deep and fixing color and ref len-->
			<xsl:apply-templates select="ProbeParts"/>
		</xsl:copy>
	</xsl:template>
	
	
	<!-- copy the ProbeParts node in deep and fixing color and ref len-->
	<xsl:template match="ProbeParts">
			<xsl:call-template name="CopyNode">
				<xsl:with-param name="node" select="current()"/>
			</xsl:call-template>
	</xsl:template>
	
	<!-- Copy a node and apply some specific rules for the attribute-->
	<xsl:template name="CopyNode">
		<xsl:param name="node"/>
		<xsl:copy>

		<xsl:for-each select="$node/@*">
				<xsl:call-template name="CopyAttribute">
					<xsl:with-param name="attribute" select="current()"/>
				</xsl:call-template>
		</xsl:for-each>
			<xsl:for-each select="$node/@*">
				<xsl:call-template name="CopyAttribute">
					<xsl:with-param name="attribute" select="current()"/>
					<xsl:with-param name="node" select="$node"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="$node/*">
				<xsl:call-template name="CopyNode">
					<xsl:with-param name="node" select="current()"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:if test="string-length($node/text()) != 0">
				<xsl:value-of select="$node/text()"/>
			</xsl:if>
		</xsl:copy>
		
	</xsl:template>
	
	<!-- copy the Attribute -->
	<xsl:template name="CopyAttribute">
		<xsl:param name="attribute"/>
		<xsl:param name="node"/>
		<xsl:choose>
		
			<xsl:when test="local-name($attribute) = 'reference_length'">
				
				<xsl:attribute name="reference_length">
					
					<xsl:choose>
						<xsl:when test="$attribute!=0">
							<xsl:value-of select="number($attribute)" />						
						</xsl:when>
						<xsl:otherwise>
							<xsl:variable name="height" select="number($node/@height)"/>
							<xsl:choose>
								<xsl:when test="boolean($height)">
									<xsl:value-of select="$height" />
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="0" />
								</xsl:otherwise>
							</xsl:choose>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:attribute>
				
			</xsl:when>
			
			<xsl:when test="local-name($attribute) = 'color'">
				<xsl:attribute name="color">
					<xsl:call-template name="BVRtoRVB">
						<xsl:with-param name="bvr" select="$attribute"/>
					</xsl:call-template>
				</xsl:attribute>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="$attribute"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	
	
	
	
	<!-- Corect the BVR to RVB -->
	<xsl:template name="BVRtoRVB">
		<xsl:param name="bvr"/>
		
		<xsl:variable name="R" select="substring($bvr,6,2)"/>
		<xsl:variable name="V" select="substring($bvr,4,2)"/>
		<xsl:variable name="B" select="substring($bvr,2,2)"/>
		<xsl:value-of select="concat('#',$R,$V,$B)"/>
		
	</xsl:template>
	
	
	
	
	
	
</xsl:stylesheet>
