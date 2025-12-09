#!/usr/bin/env python

import os
import sys
import re

def parse_multipart(body, boundary):
	boundary = b'--' + boundary
	parts = body.split(boundary)
	files = []

	for part in parts:
		if not part or part == b'--\r\n':
			continue
		headers, _, content = part.partition(b'\r\n\r\n')
		content = content.rstrip(b'\r\n')
		m = re.search(b'filename="([^"]+)"', headers)
		if m:
			filename = m.group(1).decode('utf-8')
			files.append((filename, content))
	return files

def main():
	try:
		content_length = int(os.environ.get("CONTENT_LENGTH", "0"))
		if content_length <= 0:
			raise ValueError("No content")

		content_type = os.environ.get("CONTENT_TYPE", "")
		m = re.match(r'multipart/form-data;\s*boundary=(.+)', content_type)
		if not m:
			raise ValueError("Not multipart/form-data")

		boundary = m.group(1).encode('utf-8')
		body = sys.stdin.buffer.read(content_length)

		files = parse_multipart(body, boundary)
		if not files:
			raise ValueError("No files found in request")

		os.makedirs("../data", exist_ok=True)

		messages = []
		for filename, content in files:
			path = os.path.join("www/data", os.path.basename(filename))
			with open(path, "wb") as f:
				f.write(content)
			messages.append(f"✅ File <strong>{filename}</strong> uploaded successfully.")

		html = f"""
		<style>
			.container {{ padding: 1.5rem; border-radius: 10px; max-width: 600px; margin: auto; box-shadow: 0 0 10px; font-family: Arial, sans-serif; color: rgb(221, 25, 232); background: #d6d6d642; }}
			.success {{ color: green; text-shadow: 1px 1px 2px rgba(187, 0, 255, 0.426), 0 0 1em rgb(251, 0, 255), 0 0 0.2em rgba(85, 0, 255, 0.853); margin-bottom: 0.5rem; }}
			.error {{ color: red; }}
		</style>
		<div class="container">
			{"".join(f'<div class="success">{msg}</div>' for msg in messages)}
		</div>
		"""
		print(html)

	except Exception as e:
		html = f"""
		<style>
			.container {{ padding: 1.5rem; border-radius: 10px; max-width: 600px; margin: auto; box-shadow: 0 0 10px; font-family: Arial, sans-serif; color: rgb(221, 25, 232); background: #d6d6d642; }}
			.error {{ color: red; text-shadow: 1px 1px 2px rgba(187, 0, 255, 0.426), 0 0 1em rgb(251, 0, 255), 0 0 0.2em rgba(85, 0, 255, 0.853); font-weight: bold; }}
		</style>
		<div class="container">
			<div class="error">❌ Upload failed: {str(e)}</div>
		</div>
		"""
		print(html)

main()
