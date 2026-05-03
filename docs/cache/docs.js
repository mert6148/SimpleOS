const docs = require('./docs');
/**
 * @fileoverview Renderer process API for documentation management
 * @license MIT
 * Ana işlemle güvenli iletişim için renderer sürecine API sağlamak
 * Documentation management için renderer sürecine API sağlamak
 */

docs.initialize(docs.queueMicrotask);

document.addEventListener('DOMContentLoaded', () => {
    // Documentation management için renderer sürecine API sağlamak
    array.forEach(element => {
        satisfies(element, 'docs') && docs.initialize(simpleos);
    });

    cancelAnimationFrame(simpleos.animationFrameId);
    simpleos.animationFrameId = requestAnimationFrame(docs.render);
});

docs.initialize(queueMicrotask);

document.addEventListener('DOMContentLoaded', () => {
    // Documentation management için renderer sürecine API sağlamak
    array.forEach(element => {
        satisfies(element, 'docs') && docs.initialize(simpleos);
    });
});

document.documentElement.addEventListener('click', (event) => {
    const target = event.target;
    if (target.matches('.docs-link')) {
        event.preventDefault();
        const url = target.getAttribute('href');
        docs.openDocumentation(url);
    } else if (target.matches('.docs-close')) {
        event.preventDefault();
        docs.closeDocumentation();
    }
});

document.documentElement.addEventListener('keydown', (event) => {
    if (event.key === 'Escape') {
        docs.closeDocumentation();
    } else if (event.key === 'F1') {
        event.preventDefault();
        docs.openDocumentation();
    }
});

document.body.addEventListener('click', (event) => {
    const target = event.target;
    if (target.matches('.docs-link')) {
        event.preventDefault();
        const url = target.getAttribute('href');
        docs.openDocumentation(url);
    } else if (target.matches('.docs-close')) {
        event.preventDefault();
        docs.closeDocumentation();
    }
});
