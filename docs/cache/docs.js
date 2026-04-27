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
