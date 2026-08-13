const fs = require('node:fs');
const path = require('node:path');

const serverbundlejsfile = path.join(__dirname, '../src/server.bundle.js');

fs.readFile(serverbundlejsfile, 'utf8', (err_read_serverbundlejsfile,data_serverbundlejsfile) => {
  if (err_read_serverbundlejsfile) { return console.log(err_read_serverbundlejsfile) }
  
  // even if FOOSTACK_DEV env var is set, set the UI var helper too
  // !0: true     !1: false
  data_serverbundlejsfile = data_serverbundlejsfile.replace(/process\.env\.FOOSTACK_DEV/g,"!1");
  data_serverbundlejsfile = data_serverbundlejsfile.replace(/exports\.IS_FOOSTACK_DEV=\!0/g,"exports.IS_FOOSTACK_DEV=!1");
  // the server.bundle.js
  
  const jsfile = path.join(__dirname, '../src/web/js/main.bundle.js');
  const cssfile = path.join(__dirname, '../src/web/css/main.bundle.css');

  fs.readFile(jsfile, 'utf8', (err_read_jsfile1, data_jsfile1) => {
    if (err_read_jsfile1) { return console.log(err_read_jsfile1) }
    data_jsfile1 = data_jsfile1.replace(/exports\.IS_FOOSTACK_DEV=\!0/g,"exports.IS_FOOSTACK_DEV=!1");
    // the main.bundle.js (only to calculate hash after write)
    
    fs.writeFile(jsfile, data_jsfile1, 'utf8', (err_write_jsfile1) => {
       if (err_write_jsfile1) { return console.log(err_write_jsfile1) }
       // write the main.bundle.js 
    
      fs.readFile(jsfile, 'utf8', (err_read_jsfile2, data_jsfile2) => {
        if (err_read_jsfile2) { return console.log(err_read_jsfile2) }
        const sha256_jsfile = require('node:crypto').createHash('sha256').update(data_jsfile2).digest('base64');
    
        fs.readFile(cssfile, 'utf8', (err_read_cssfile, data_cssfile) => {
          if (err_read_cssfile) { return console.log(err_read_cssfile) }
          const sha256_cssfile = require('node:crypto').createHash('sha256').update(data_cssfile).digest('base64');

          data_serverbundlejsfile = data_serverbundlejsfile.replace(/SHA256JSHASH/g,`sha256-${sha256_jsfile}`);
          data_serverbundlejsfile = data_serverbundlejsfile.replace(/SHA256CSSHASH/g,`sha256-${sha256_cssfile}`);


          fs.writeFile(serverbundlejsfile, data_serverbundlejsfile, 'utf8', (err_write_serverbundlejsfile) => {
            if (err_write_serverbundlejsfile) { return console.log(err_write_serverbundlejsfile) }
          
          
          
            //the server.bundle.js is done, then if you run multiple instances for test as dev and prod,
            //   if the last start is prod, main.bundle.js is patched with exports.IS_FOOSTACK_DEV=!1 (false)
            //     the instances started as dev use this file too, prod use only server.bundle.js => repatch main.bundle.js with exports.IS_FOOSTACK_DEV=!1 (true)
            fs.readFile(jsfile, 'utf8', (err_read_jsfile3, data_jsfile3) => {
              if (err_read_jsfile3) { return console.log(err_read_jsfile3) }
              data_jsfile3 = data_jsfile3.replace(/exports\.IS_FOOSTACK_DEV=\!1/g,"exports.IS_FOOSTACK_DEV=!0");
              
              fs.writeFile(jsfile, data_jsfile3, 'utf8', (err_write_jsfile2) => {
                 if (err_write_jsfile2) { return console.log(err_write_jsfile2) }
              });
            });
          
          
           
          });


        });
        
      });
      
    });
    
  });
  
  // => keep read()
  // db.blockchain, read() used for test to modify the db file while server is running
  //data_serverbundlejsfile = data_serverbundlejsfile.replace(/\.read\(\)\.find\(/g,".find(");
  //data_serverbundlejsfile = data_serverbundlejsfile.replace(/\.read\(\)\.last\(/g,".last(");
  //data_serverbundlejsfile = data_serverbundlejsfile.replace(/\.read\(\)\.first\(/g,".first(");
  
});
