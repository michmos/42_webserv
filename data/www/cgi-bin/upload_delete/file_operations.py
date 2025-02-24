#!/usr/bin/env python3

import sys
import os

class UploadFile:
	def __init__(self, filename):
		self.file = filename
		self.path = ""
		
	def is_direction( self, path ) -> bool:
		return (os.path.exists(path))

	def create_directory( self, path ) -> int:
		try:
			os.umask(0)
			os.makedirs(path, mode=0o777, exist_ok=True)
			return 200
		except PermissionError:
			sys.stderr.write(f"Permission denied: Unable to create '{path}'.")
		except Exception as e:
			sys.stderr.write(f"Error creating_directory: {e}")
		return 500

	def prepare_path( self ) -> int:
		base_path = "data/uploads/"
		status_code = 200
		if not self.is_direction(base_path):
			status_code = self.create_directory(base_path)
		self.path = os.path.join(base_path, self.file)
		return status_code

	def write_to_file( self, filename, output ):
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

class DeleteFile:
	def __init__( self, filename ):
		print(os.getcwd(), file=sys.stderr)
		self.file = "data/uploads" + filename
	
	def is_existing_file( self ) -> bool:
		print("to delete:", self.file, file=sys.stderr)
		if os.path.isfile(self.file):
			return True 
		else:
			return False
	
	def delete_file( self ) -> int:
		if self.is_existing_file():
			os.remove(self.file)
			return 200
		print("file does not exist", file=sys.stderr)
		return 404
