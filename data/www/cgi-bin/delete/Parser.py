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
					print("q", q, file=sys.stderr)
					if found == True:
						file = q
						break
					if q == 'file':
						found = True
		elif request_method == 'DELETE' and "REQUEST_TARGET" in os.environ:
			file = os.getenv('REQUEST_TARGET')
			if "../" in file:
				self.status_code = 403
		else:
			file = ""
		return file