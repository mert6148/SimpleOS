const kernel = {
  name: 'kernel',
  version: '0.1.0',
  description: 'A kernel for running code in a sandboxed environment.',
  main: 'index.js',
  scripts: {
    start: 'node index.js',
    test: 'echo "Error: no test specified" && exit 1'
  },
}

document.addEventListener('DOMContentLoaded', () => {
  console.log('Kernel loaded');
});

module.exports = kernel;