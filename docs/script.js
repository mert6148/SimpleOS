// ============================================================
// Theme Toggle
// ============================================================
const themeToggle = document.getElementById('themeToggle');
const htmlElement = document.documentElement;
const darkModeKey = 'simpleos-dark-mode';

// Initialize theme from localStorage
function initializeTheme() {
    const prefersDark = localStorage.getItem(darkModeKey);
    const prefersDarkMedia = window.matchMedia('(prefers-color-scheme: dark)').matches;
    
    const isDark = prefersDark === 'true' || (prefersDark === null && prefersDarkMedia);
    
    if (isDark) {
        document.body.classList.add('dark-mode');
        updateThemeIcon();
    }
}

// Update theme icon
function updateThemeIcon() {
    const isDark = document.body.classList.contains('dark-mode');
    themeToggle.innerHTML = isDark ? '<i class="fas fa-sun"></i>' : '<i class="fas fa-moon"></i>';
}

// Toggle theme
themeToggle.addEventListener('click', () => {
    document.body.classList.toggle('dark-mode');
    const isDark = document.body.classList.contains('dark-mode');
    localStorage.setItem(darkModeKey, isDark);
    updateThemeIcon();
});

// Initialize theme on page load
initializeTheme();

// ============================================================
// API Tabs
// ============================================================
const apiTabButtons = document.querySelectorAll('.api-tab-button');
const apiTabs = document.querySelectorAll('.api-tab');

apiTabButtons.forEach(button => {
    button.addEventListener('click', () => {
        const tabName = button.getAttribute('data-tab');
        
        // Remove active class from all buttons and tabs
        apiTabButtons.forEach(btn => btn.classList.remove('active'));
        apiTabs.forEach(tab => tab.classList.remove('active'));
        
        // Add active class to clicked button and corresponding tab
        button.classList.add('active');
        const activeTab = document.getElementById(`${tabName}-tab`);
        if (activeTab) {
            activeTab.classList.add('active');
        }
    });
});

// ============================================================
// Smooth Scroll for Navigation Links
// ============================================================
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        const href = this.getAttribute('href');
        if (href !== '#') {
            e.preventDefault();
            const target = document.querySelector(href);
            if (target) {
                target.scrollIntoView({
                    behavior: 'smooth',
                    block: 'start'
                });
            }
        }
    });
});

// ============================================================
// Active Navigation Link
// ============================================================
function updateActiveNavLink() {
    const sections = document.querySelectorAll('section[id]');
    const navLinks = document.querySelectorAll('.navbar a');
    
    let currentSection = '';
    
    sections.forEach(section => {
        const sectionTop = section.offsetTop;
        const sectionHeight = section.clientHeight;
        
        if (window.pageYOffset >= sectionTop - 100) {
            currentSection = section.getAttribute('id');
        }
    });
    
    navLinks.forEach(link => {
        link.classList.remove('active');
        const href = link.getAttribute('href').slice(1);
        if (href === currentSection) {
            link.classList.add('active');
        }
    });
}

window.addEventListener('scroll', updateActiveNavLink);

// ============================================================
// Contact Form Handling
// ============================================================
const contactForm = document.querySelector('.contact-form form');

if (contactForm) {
    contactForm.addEventListener('submit', function (e) {
        e.preventDefault();
        
        const name = this.querySelector('input[type="text"]').value;
        const email = this.querySelector('input[type="email"]').value;
        const subject = this.querySelectorAll('input[type="text"]')[1].value;
        const message = this.querySelector('textarea').value;
        
        // Basic validation
        if (!name || !email || !subject || !message) {
            alert('Please fill in all fields');
            return;
        }
        
        // Email validation
        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
        if (!emailRegex.test(email)) {
            alert('Please enter a valid email address');
            return;
        }
        
        // Create mailto link
        const mailtoLink = `mailto:mertdogan00ay437@gmail.com?subject=${encodeURIComponent(subject)}&body=${encodeURIComponent(`From: ${name} (${email})\n\n${message}`)}`;
        
        // Open default email client
        window.location.href = mailtoLink;
        
        // Reset form
        setTimeout(() => {
            this.reset();
        }, 500);
    });
}

// ============================================================
// Add Animation Classes on Scroll
// ============================================================
const observerOptions = {
    threshold: 0.1,
    rootMargin: '0px 0px -100px 0px'
};

const observer = new IntersectionObserver(function(entries) {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.style.opacity = '1';
            entry.target.style.transform = 'translateY(0)';
        }
    });
}, observerOptions);

// Observe feature cards
document.querySelectorAll('.feature-card, .arch-card, .example-card').forEach(card => {
    card.style.opacity = '0';
    card.style.transform = 'translateY(20px)';
    card.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
    observer.observe(card);
});

// ============================================================
// Navbar Styles
// ============================================================
const navbarLinks = document.querySelectorAll('.navbar a');

navbarLinks.forEach(link => {
    link.addEventListener('mouseenter', function() {
        this.style.opacity = '0.8';
    });
    
    link.addEventListener('mouseleave', function() {
        this.style.opacity = '1';
    });
});

// ============================================================
// Copy Code to Clipboard
// ============================================================
const codeBlocks = document.querySelectorAll('.code-block');

codeBlocks.forEach(block => {
    // Create copy button
    const copyButton = document.createElement('button');
    copyButton.className = 'copy-btn';
    copyButton.innerHTML = '<i class="fas fa-copy"></i> Copy';
    copyButton.style.cssText = `
        position: absolute;
        top: 10px;
        right: 10px;
        background: #667eea;
        color: white;
        border: none;
        padding: 0.5rem 1rem;
        border-radius: 6px;
        cursor: pointer;
        font-size: 0.85rem;
        opacity: 0;
        transition: opacity 0.3s ease;
    `;
    
    // Position block relative if not already
    if (window.getComputedStyle(block).position === 'static') {
        block.style.position = 'relative';
    }
    
    block.appendChild(copyButton);
    
    // Show copy button on hover
    block.addEventListener('mouseenter', () => {
        copyButton.style.opacity = '1';
    });
    
    block.addEventListener('mouseleave', () => {
        copyButton.style.opacity = '0';
    });
    
    // Copy to clipboard
    copyButton.addEventListener('click', (e) => {
        e.preventDefault();
        const code = block.querySelector('code') || block.querySelector('pre');
        const text = code ? code.innerText : block.innerText;
        
        navigator.clipboard.writeText(text).then(() => {
            copyButton.innerHTML = '<i class="fas fa-check"></i> Copied!';
            setTimeout(() => {
                copyButton.innerHTML = '<i class="fas fa-copy"></i> Copy';
            }, 2000);
        }).catch(err => {
            console.error('Failed to copy:', err);
        });
    });
});

// ============================================================
// Keyboard Shortcuts
// ============================================================
document.addEventListener('keydown', (e) => {
    // Press 'T' to toggle theme
    if (e.key === 't' && e.ctrlKey) {
        e.preventDefault();
        themeToggle.click();
    }
});

// ============================================================
// Page Load Animations
// ============================================================
window.addEventListener('load', () => {
    document.body.style.opacity = '1';
    
    // Animate hero content
    const heroContent = document.querySelector('.hero-content');
    if (heroContent) {
        heroContent.style.animation = 'fadeInUp 0.8s ease forwards';
    }
});

// Add CSS animations
const style = document.createElement('style');
style.textContent = `
    @keyframes fadeInUp {
        from {
            opacity: 0;
            transform: translateY(30px);
        }
        to {
            opacity: 1;
            transform: translateY(0);
        }
    }
`;
document.head.appendChild(style);
    for (let i = 0; i < items.length; i++) {
        const item = items[i];
        if (item.textContent.toLowerCase().includes(query)) {
            item.style.display = "";
        } else {
            item.style.display = "none";
        }
    }
});