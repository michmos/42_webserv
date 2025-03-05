#!/usr/bin/env python3

from ResponseGenerator import ResponseGenerator
import os
import sys

def protocol_check() -> int:
	try:
		assert "CGI/1.1" == os.getenv("GATEWAY")
		assert "HTTP/1.1" == os.getenv("SERVER_PROTOCOL")
		return 200
	except Exception as e:
		print("Wrong protocol used", file=sys.stderr)
		return 500

def correct_use_path_info( path_info_needed:bool ) -> bool:
	if "PATH_INFO" in os.environ and path_info_needed == True:
		print("info and needed", file=sys.stderr)
		return True
	elif "PATH_INFO" not in os.environ and path_info_needed == False:
		print("no info and not needed", file=sys.stderr)
		return True
	else:
		return False

def send_error_header( request_method:str, status_code:int ):
	rg = ResponseGenerator(request_method, status_code)
	response = rg.create_response()
	sys.stdout.write(response)
	sys.stdout.flush()

def general_checks( path_info_needed:bool ) -> bool:
	if "REQUEST_METHOD" not in os.environ:
		print("No request method", file=sys.stderr)
		send_error_header( "None", 400)
		return True
	method = os.getenv("REQUEST_METHOD")
	status_code = protocol_check()
	if status_code != 200:
		send_error_header(method, status_code)
		return True
	if correct_use_path_info(path_info_needed) == False:
		send_error_header(method, 404)
		return True
	return False