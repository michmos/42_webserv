#!/usr/bin/env python3

import sys
import os

class UploadFile:
	def __init__(self, filename:str ):
		self.file = filename
		self.path = ""
		
	def is_direction( self, path:str ) -> bool:
		return (os.path.exists(path))

	def create_directory( self, path:str ) -> int:
		try:
			os.umask(0)
			os.makedirs(path, mode=0o777, exist_ok=True)
			return 200
		except PermissionError:
			print(f"Permission denied: Unable to create '{path}'.", file=sys.stderr)
		except Exception as e:
			print(f"Error creating_directory: {e}", file=sys.stderr)
		return 500

	def prepare_path( self ) -> int:
		base_path = "data/uploads/"
		status_code = 200
		if not self.is_direction(base_path):
			status_code = self.create_directory(base_path)
		self.path = os.path.join(base_path, self.file)
		return status_code

	def write_to_file( self, filename:str , output:str ):
		try:
			if (filename[-3:] == "txt"):
				with open(filename, "w") as f:
					output = output.decode("utf-8")
					f.write(output)
			else:
				with open(filename, "wb") as f:
					f.write(output)
		except Exception as e:
			sys.stderr.write(f"Error write_to_file: {e}")
			return 500
		return 200

	def upload_file( self, post_data ) -> int:
		status_code = self.prepare_path()
		if status_code != 200:
			return status_code
		return self.write_to_file(self.path, post_data)
