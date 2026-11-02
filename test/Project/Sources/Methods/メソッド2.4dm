//%attributes = {}
C_COLLECTION:C1488($col)
$col:=New collection:C1472()

While (True:C214)
	$col.push(New collection:C1472(1000000))  // Keep allocating memory
End while 