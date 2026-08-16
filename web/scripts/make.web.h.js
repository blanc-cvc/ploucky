const fs = require('node:fs');
const path = require('node:path');

const htmlbundlefile = path.join(__dirname, '../_generated/index.bundle.html');
const webhfile = path.join(__dirname, '../../modules/_generated/web.h');
const htmlfile = path.join(__dirname, '../index.html');
const jsfile = path.join(__dirname, '../_generated/main.bundle.js');
const cssfile = path.join(__dirname, '../_generated/main.bundle.css');

function split_html(str, maxLen) {
    const chunks = [];
    let start = 0;
    
    while (start < str.length) {
        let end = start + maxLen;
        
        if (end >= str.length) {
            chunks.push(str.substring(start));
            break;
        }

        while (end > start && str[end - 1] === '\\') {
            end--;
        }

        while (end > start) {
            if (str[end - 1] !== '\\') {
                break;
            }
            if (end > 1 && str[end - 2] === '\\') {
                break;
            }
            end--;
        }
        
        chunks.push(str.substring(start, end));
        start = end;
    }
    
    return chunks;
}

fs.readFile(htmlfile, 'utf8', (err_read_htmlfile,data_htmlfile) => {
  if (err_read_htmlfile) { return console.log(err_read_htmlfile) }
  
  fs.readFile(jsfile, 'utf8', (err_read_jsfile,data_jsfile) => {
    if (err_read_jsfile) { return console.log(err_read_jsfile) }
    
    fs.readFile(cssfile, 'utf8', (err_read_cssfile,data_cssfile) => {
      if (err_read_cssfile) { return console.log(err_read_cssfile) }
      
      const sha256_jsfile = require('node:crypto').createHash('sha256').update(data_jsfile).digest('base64');
      const sha256_cssfile = require('node:crypto').createHash('sha256').update(data_cssfile).digest('base64');
      
      data_htmlfile = data_htmlfile.replace(/<!--[\s\S]*?-->/g, '').replace(/>\s+/g, '>').replace(/\s+</g, '<'); // search.brave.com: minify html
      data_htmlfile = data_htmlfile.replace("<script id=\"js\" src=\"/main.js\"></script>", `<script id="js">${data_jsfile}</script>`);
      data_htmlfile = data_htmlfile.replace("<link id=\"css\" href=\"/main.css\" rel=\"stylesheet\">", `<style id="css">${data_cssfile}</style>`);

      
      
      fs.writeFile(htmlbundlefile, data_htmlfile, 'utf8', (err_write_htmlbundlefile) => {
        if (err_write_htmlbundlefile) { return console.log(err_write_htmlbundlefile) }
       
        
        
        // helper: convert html file to inline string
        const obj = { html: ""};
        obj.html = data_htmlfile;
        data_htmlfile = JSON.stringify(obj);
        const test = JSON.parse(data_htmlfile);
        data_htmlfile = data_htmlfile.replace(/{"html":"/g,"");
        data_htmlfile = data_htmlfile.slice(0, -2);
        
        const MAX_LEN = 4000;
        const chunks = split_html(data_htmlfile, MAX_LEN);
        const chunks_map = chunks.map(chunk => `    "${chunk}"`).join('\n');        
        const datawebhfile = `// MODULES / _GENERATED / WEB.H (auto-generated do not edit)

#ifndef GENERATED_WEB_H
  #define GENERATED_WEB_H

  // https://github.com/helmetjs/helmet
  const char HTTP_STATIC_HEADERS[] = 
    "Cache-Control: no-cache, no-store, must-revalidate\\r\\n"
    "Pragma: no-cache\\r\\n"
    "Expires: 0\\r\\n"
    "Content-Security-Policy: default-src 'self'; base-uri 'self'; font-src 'self'; form-action 'self'; frame-ancestors 'self'; img-src 'self' data:; object-src 'none'; script-src 'self' 'sha256-${sha256_jsfile}'; script-src-attr 'none'; style-src 'self' 'sha256-${sha256_cssfile}'; worker-src 'self' blob:\\r\\n"
    "Cross-Origin-Opener-Policy: same-origin\\r\\n"
    "Cross-Origin-Resource-Policy: same-origin\\r\\n"
    "Origin-Agent-Cluster: ?1\\r\\n"
    "Referrer-Policy: no-referrer\\r\\n"
    "X-Content-Type-Options: nosniff\\r\\n"
    "X-DNS-Prefetch-Control: off\\r\\n"
    "X-Download-Options: noopen\\r\\n"
    "X-Frame-Options: DENY\\r\\n"
    "X-Permitted-Cross-Domain-Policies: none\\r\\n"
    "X-XSS-Protection: 0\\r\\n";
  
  const char WEB_PAGE_HTML[] = 
${chunks_map};

#endif
`;
        
        
        fs.writeFile(webhfile, datawebhfile, 'utf8', (err_write_webhfile) => {
          if (err_write_webhfile) { return console.log(err_write_webhfile) }
        });
        
       
      });

    });
    
  });
  
});

