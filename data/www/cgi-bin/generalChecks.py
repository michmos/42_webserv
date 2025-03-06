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
		return True
	elif "PATH_INFO" not in os.environ and path_info_needed == False:
		return True
	else:
		return False

def send_error_header( request_method:str, status_code:int ):
	rg = ResponseGenerator(request_method, status_code)
	response = rg.create_response()
	sys.stdout.write(response)
	sys.stdout.flush()

def wrong_segment( env:str ) -> bool:
	path = os.getenv(env)
	if "//" in path or "/./" in path or "/../" in path:
		return True
	else:
		return False

def wrong_use_of_path_segments() -> bool:
	""" Checks PATH_INFO, PATH_TRANSLATED and SCRIPT_NAME if provided unsafe use of //, . or .., not allowed in our CGI"""
	if "PATH_INFO" in os.environ and wrong_segment("PATH_INFO"):
		print("Wrong segment in PATH_INFO", file=sys.stderr)
		return True
	if "PATH_TRANSLATED" in os.environ and wrong_segment("PATH_TRANSLATED"):
		print("Wrong segment in PATH_TRANSLATED", file=sys.stderr)
		return True
	if "SCRIPT_NAME" in os.environ and wrong_segment("SCRIPT_NAME"):
		print("Wrong segment in SCRIPT_NAME", file=sys.stderr)
		return True
	return False

def general_checks( path_info_needed:bool ) -> bool:
	""" Checks for request method, improper path manipulation and protocol version """
	if "REQUEST_METHOD" not in os.environ:
		print("No request method", file=sys.stderr)
		send_error_header( "None", 400)
		return True
	if wrong_use_of_path_segments():
		send_error_header( os.getenv("REQUEST_METHOD"), 404)
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
	