@import url('https://fonts.googleapis.com/css2?family=VT323&family=Nunito:wght@400;600;700;800&display=swap');
:root{--sidebar-w:200px;--bg:#0f1112;--bg2:#161a1e;--bg3:#1e2329;--bg4:#252c35;--border:#2a333d;--border2:#364555;--gold:#f0c040;--gold2:#a07820;--purple:#9d56ff;--blue:#4d8fff;--green:#4ec97e;--red:#ff5252;--orange:#ff8c42;--gray:#6b7a8d;--gray2:#8a9bae;--white:#e8edf2;--radius:6px;}
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0;}
html,body{height:100%;}
body{background:var(--bg);color:var(--white);font-family:'Nunito',sans-serif;font-size:13px;display:flex;overflow:hidden;}
::-webkit-scrollbar{width:4px;height:4px;}::-webkit-scrollbar-track{background:transparent;}::-webkit-scrollbar-thumb{background:var(--border2);border-radius:2px;}
*{scrollbar-width:thin;scrollbar-color:var(--border2) transparent;}

/* SIDEBAR */
#sidebar{width:var(--sidebar-w);flex-shrink:0;background:var(--bg2);border-right:1px solid var(--border);display:flex;flex-direction:column;height:100vh;overflow:hidden;}
.sidebar-logo{padding:13px 12px;border-bottom:1px solid var(--border);display:flex;align-items:center;gap:9px;text-decoration:none;flex-shrink:0;}
.sidebar-logo-icon{font-size:20px;}
.sidebar-logo-text{font-family:'VT323',monospace;font-size:20px;color:var(--gold);letter-spacing:1px;line-height:1;}
.sidebar-logo-sub{font-size:9px;color:var(--gray);letter-spacing:.3px;margin-top:1px;}
.sidebar-nav{flex:1;padding:8px 6px;overflow-y:auto;display:flex;flex-direction:column;gap:1px;}
.sidebar-section{font-size:9px;font-weight:800;text-transform:uppercase;letter-spacing:1.2px;color:var(--border2);padding:9px 6px 3px;}
.nav-item{display:flex;align-items:center;gap:8px;padding:7px 9px;border-radius:var(--radius);border:1px solid transparent;color:var(--gray);font-size:12px;font-weight:700;text-decoration:none;transition:all .1s;position:relative;user-select:none;}
.nav-item:hover{background:var(--bg3);color:var(--white);}
.nav-item.active{background:rgba(240,192,64,.07);border-color:rgba(240,192,64,.18);color:var(--gold);}
.nav-item.active::before{content:'';position:absolute;left:0;top:50%;transform:translateY(-50%);width:2px;height:55%;background:var(--gold);border-radius:0 2px 2px 0;}
.nav-item.disabled{opacity:.28;pointer-events:none;}
.nav-item-icon{font-size:14px;width:18px;text-align:center;flex-shrink:0;}
.nav-item-label{flex:1;}
.nav-item-badge{font-size:8px;font-weight:800;text-transform:uppercase;background:rgba(255,255,255,.04);border:1px solid var(--border);color:var(--gray);border-radius:3px;padding:1px 5px;}
.sidebar-auth{flex-shrink:0;border-top:1px solid var(--border);padding:8px;}
.auth-widget{display:flex;align-items:center;gap:8px;padding:7px 8px;border-radius:var(--radius);background:var(--bg3);border:1px solid var(--border);text-decoration:none;transition:border-color .1s;}
.auth-widget:hover{border-color:var(--border2);}
.auth-avatar{width:26px;height:26px;border-radius:50%;background:var(--gold2);display:flex;align-items:center;justify-content:center;font-size:12px;font-weight:800;color:var(--gold);flex-shrink:0;border:1px solid var(--gold2);overflow:hidden;}
.auth-avatar img{width:100%;height:100%;object-fit:cover;}
.auth-info{flex:1;min-width:0;}
.auth-name{font-size:11px;font-weight:700;color:var(--white);white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}
.auth-status{font-size:9px;color:var(--gray);}
.auth-arrow{font-size:10px;color:var(--border2);}
.sidebar-version{padding:6px 12px 8px;font-size:9px;color:var(--border2);display:flex;align-items:center;gap:5px;}
.version-dot{width:5px;height:5px;border-radius:50%;background:var(--green);box-shadow:0 0 5px var(--green);}

/* APP */
#app{flex:1;display:flex;flex-direction:column;overflow:hidden;min-width:0;}
#page-content{flex:1;overflow-y:auto;overflow-x:hidden;min-height:0;}

/* FOOTER — compact single bar */
#footer{flex-shrink:0;background:var(--bg2);border-top:1px solid var(--border);}
.footer-inner{display:none;} /* contenu géré dans footer-bottom */
.footer-bottom{
  padding:8px 20px;display:flex;align-items:center;gap:8px;flex-wrap:wrap;
  font-size:10px;color:var(--border2);
}
.footer-logo-text{font-family:'VT323',monospace;font-size:14px;color:var(--gold2);letter-spacing:.5px;margin-right:4px;}
.footer-sep{color:var(--border);}
.footer-col a{font-size:10px;color:var(--border2);text-decoration:none;transition:color .1s;}
.footer-col a:hover{color:var(--gray2);}
.footer-spacer{flex:1;}

/* BUTTONS */
.btn{display:inline-flex;align-items:center;gap:6px;padding:8px 16px;border-radius:var(--radius);border:1px solid transparent;font-size:12px;font-weight:700;font-family:'Nunito',sans-serif;cursor:pointer;transition:all .12s;text-decoration:none;white-space:nowrap;}
.btn-primary{background:var(--gold);color:#1a1000;}
.btn-primary:hover{background:#f5cc55;box-shadow:0 0 16px rgba(240,192,64,.25);}
.btn-secondary{background:var(--bg3);color:var(--white);border-color:var(--border2);}
.btn-secondary:hover{background:var(--bg4);}
.btn-ghost{background:transparent;color:var(--gray);border-color:var(--border);}
.btn-ghost:hover{background:var(--bg3);color:var(--white);}
.btn-danger{background:rgba(255,82,82,.08);color:var(--red);border-color:rgba(255,82,82,.25);}
.btn-danger:hover{background:rgba(255,82,82,.15);}
.btn-sm{padding:5px 11px;font-size:11px;}
.btn-lg{padding:10px 22px;font-size:14px;}
.input{background:var(--bg);border:1px solid var(--border);border-radius:var(--radius);padding:8px 12px;color:var(--white);font-size:12px;font-family:'Nunito',sans-serif;outline:none;transition:border-color .12s;width:100%;}
.input::placeholder{color:var(--border2);}
.input:focus{border-color:var(--purple);}
.input-label{font-size:9px;font-weight:800;color:var(--gray);margin-bottom:4px;display:block;text-transform:uppercase;letter-spacing:.8px;}
select.input{cursor:pointer;}select.input option{background:var(--bg3);}
.card{background:var(--bg2);border:1px solid var(--border);border-radius:var(--radius);}
.card:hover{border-color:var(--border2);}
.badge{display:inline-flex;align-items:center;gap:3px;padding:2px 7px;border-radius:3px;font-size:9px;font-weight:800;text-transform:uppercase;letter-spacing:.5px;}
.badge-gold{background:rgba(240,192,64,.1);border:1px solid rgba(240,192,64,.25);color:var(--gold);}
.badge-green{background:rgba(78,201,126,.1);border:1px solid rgba(78,201,126,.25);color:var(--green);}
.badge-blue{background:rgba(77,143,255,.1);border:1px solid rgba(77,143,255,.25);color:var(--blue);}
.badge-gray{background:rgba(255,255,255,.04);border:1px solid var(--border);color:var(--gray);}
.section-title{font-size:9px;font-weight:800;text-transform:uppercase;letter-spacing:1.2px;color:var(--border2);margin-bottom:12px;display:flex;align-items:center;gap:8px;}
.section-title::after{content:'';flex:1;height:1px;background:var(--border);}
#toast-container{position:fixed;bottom:16px;right:16px;display:flex;flex-direction:column;gap:6px;z-index:9999;}
.toast{background:var(--bg3);border:1px solid var(--border2);border-radius:var(--radius);padding:9px 14px;font-size:12px;font-weight:600;display:flex;align-items:center;gap:7px;box-shadow:0 4px 16px rgba(0,0,0,.4);animation:toastIn .18s ease;min-width:200px;}
.toast.success{border-color:rgba(78,201,126,.4);color:var(--green);}
.toast.error{border-color:rgba(255,82,82,.4);color:var(--red);}
.toast.info{border-color:rgba(77,143,255,.4);color:var(--blue);}
@keyframes toastIn{from{opacity:0;transform:translateX(12px)}to{opacity:1;transform:none}}
