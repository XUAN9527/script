#$language = "VBScript"
#$interface = "1.0"

crt.Screen.Synchronous = True

' This automatically generated script may need to be
' edited in order to work correctly.

Sub Main
'	str_propt = "successful"
'	cmd_version = "factory 1003"
'	factory_cmd = "factory 1001"
'	cmd_upgrade = "factory 1002"
'	last_version = "VERSION=0.15"
	file_path = ".\papp.bin"
	While True
'		crt.Screen.Send factory_cmd & chr(13)& chr(10)	'等待设备回复successful
'		ret = crt.Screen.WaitForString(str_propt,5)
		ret = True
		If ret <> 0 Then
'			crt.Screen.Send cmd_version & chr(13)& chr(10)	'查询设备当前版本
'			ret = crt.Screen.WaitForString(last_version,5)'5秒内没有收到last_version，则认为不是最新版本，执行升级
			ret = False
			If ret <> 0 Then
				crt.Dialog.MessageBox("当前已经是最新版本")
			Else
'				crt.Screen.Send cmd_upgrade & chr(13)& chr(10)
				ret = crt.Screen.WaitForString("CC", 10)
				If ret <> 0 Then
					crt.FileTransfer.ClearUploadList
					crt.FileTransfer.AddToUploadList file_path
					crt.FileTransfer.SendYmodem
					
'					ret = crt.Screen.WaitForString(last_version, 20)
'					If ret <> 0 Then
'						crt.Sleep 1000
'					Else
'						crt.Dialog.MessageBox("升级失败")
'					End if
					crt.Dialog.MessageBox("升级成功")
				End if
			End if
		End if
	Wend
End Sub
