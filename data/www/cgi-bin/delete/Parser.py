#!/usr/bin/env python3

import os
import urllib.parse
import cgi
import sys
import re

class Parser:
	def __init__( self ):
		self.status_code = 200


	def get_status_code( self ) -> int:
		return self.status_code


	def parsing_cgi_input( self ) -> str:
		"""
			Check method == DELETE request
			Returns filename extract from request target
		"""
		if "REQUEST_METHOD" not in os.environ:
			print("Error: no request method", file=sys.stderr)
			self.status_code = 400
			return

		request_method = os.getenv('REQUEST_METHOD')
		if  request_method != "DELETE" and request_method != "GET":
			print(os.getenv('REQUEST_METHOD'), file=sys.stderr)
			print("Error: wrong request method", file=sys.stderr)
			self.status_code = 400
			return
			
		if request_method == "GET":
			if 'QUERY_STRING' in os.environ:
				query_string = os.getenv('QUERY_STRING')
				queries = query_string.split('=')
				found = False
				for q in queries:
					if found == True:
						file = q
						break
					if q == 'file':
						found = True
		elif request_method == 'DELETE' and "DELETE_FILE" in os.environ:
			file = os.getenv('DELETE_FILE')
			if "../" in file:
				self.status_code = 403
			file = os.path.basename(file)
			if "SERVER_PROTOCOL" in os.environ and os.getenv("SERVER_PROTOCOL") == "HTTP/1.1":
				0
			else:
				print("No or wrong SERVER_PROTOCOL", file=sys.stderr)
				self.status_code = 400
		else:
			file = ""
		return file