async function delete_req() {
	const input = document.getElementById('delete-input');
	const filename = input && input.value.trim();
	if (!filename) {
		alert('Veuillez indiquer le nom du fichier à supprimer.');
		return;
	}
	fetch('/' + encodeURIComponent(filename), { method: 'DELETE' })
	.then(async res => {
		if (res.ok) {
			input.value = '';
			alert('File ' + filename + ' deleted');
		} else {
			const text = await res.text().catch(() => '');
			alert('Delete failed: ' + (text || res.status));
		}
	})
}

async function fetchCgiList() {
	try {
		const res = await fetch('/cgi/list');
		if (!res.ok) throw new Error('Failed to fetch CGI list');
		const list = await res.json();
		const sel = document.getElementById('cgi-list');
		sel.innerHTML = '<option value="" selected disabled hidden>Select CGI</option>';
		list.forEach(name => {
			const opt = document.createElement('option');
			opt.value = name;
			opt.textContent = name;
			sel.appendChild(opt);
		});
	} catch (err) {
		console.error(err);
		alert('Could not load CGI list');
	}
}

document.addEventListener('DOMContentLoaded', function() {
  const form = document.getElementById('cgi-forms');
  if (!form) return;

  form.addEventListener('submit', function(event) {
	event.preventDefault();
	const selected = document.getElementById('cgi-list').value;
	if (!selected) return;
	window.location.href = `/cgi-bin/${encodeURIComponent(selected)}`;
  });
});