Die Path-Variable OPENCV_DIR muss gesetzt werden, zB.: C:\opencv2.13\opencv\build\x64\vc12.
Nachdem ist die Applikation ausführbar.
In config.txt sind alle benutze Variablen parametrisiert.
Die Variablen die den Algorithmus konfigurieren wurden in der Arbeit erwähnt.
Sie sind in der Reihenfolge:

5.1:
borderSize
lowerBorderCorr
upperBorderCorr
leftBorderCorr
rightBorderCorr
borderSteps
5.2:
findStringBinaryTh
minWordWidth
minWordHeight
minAvgHeight
maxAvgHeight
5.3:
resamplingTh

Die Parameter:
wantOrgImportanceMap
wantBlurredImage
wantTextImage
wantFinalResult
wantImportanceMap
wantDebugSeams
wantHists
wantSizeOutput
sind Boolische Variablen und regeln, welche Output gegeben werden sollte.
wantOrgImportanceMap schreibt die originelle Importancemap aus, wantImportanceMap die nach seam carving.
wantDebugSeams schreibt die besten Seams nach dem ersten Durchlauf aus, wobei die rote tatsächlich eliminiert wird.
wantHists zeigt die Histogramme von UI-Elimination an. Es kann aber vorkommen, dass kein Output generiert wird, weil die vorige Heuristiken es vermeiden (es wurde schon genug weggeschnitten oder die beide Histogramme sind nicht unterschiedlich genug).

Die restlichen Variablen sind für Debuging benutzt bzw. konfigurieren OpenCV Funktionen wie Filterkernel und Histograms.
Die größe von Output ist einstellbar mit der 'size' Variable.

Die default config.txt schaut wie folg aus:

kernelSizeLaplace=3
scaleLaplace=1
deltaLaplace=0
horizontalLineOffset=30
convertBinaryTh=200
blurWidth=15
blurHeight=1
findStringBinaryTh=100
minWordWidth=5
minWordHeight=5
minAvgHeight=0.5
maxAvgHeight=10
histSize=10
minHistRange=0
maxHistRange=256
histTh=0.65
wordWeight=150
borderSize=0.2
borderSteps=0.025
lowerBorderCorr=1.3
upperBorderCorr=2.0
leftBorderCorr=2.0
rightBorderCorr=1.3
debugSeamAmount=1000
size=0.5
resamplingTh=0.5
doLines=0
wantOrgImportanceMap=0
wantBlurredImage=0
wantTextImage=0
wantFinalResult=1
wantImportanceMap=0
wantDebugSeams=0
wantHists=0
wantSizeOutput=0