"use strict";
const NAV_ITEMS=[
  {id:'seedmap',label:'Carte Seed',icon:'🗺️',href:'seedmap.html',ready:true},
  {id:'enchantments',label:'Enchantements',icon:'✨',href:'enchantments.html',ready:true},
  {id:'food',label:'Nourriture',icon:'🍖',href:'food.html',ready:true},
  {id:'beacon',label:'Couleur Balise',icon:'🔦',href:'beacon.html',ready:true},
  {id:'crafts',label:'Recettes Craft',icon:'🔨',href:'crafts.html',ready:false},
  {id:'farms',label:'Farm Calculator',icon:'⚙️',href:'farms.html',ready:false},
];
function getCurrentPage(){return(window.location.pathname.split('/').pop()||'index.html').replace('.html','');}
function buildSidebar(){
  const cur=getCurrentPage();
  const items=NAV_ITEMS.map(i=>{
    const a=i.id===cur?'active':'',d=i.ready?'':'disabled',b=i.ready?'':'<span class="nav-item-badge">WIP</span>';
    return`<a href="${i.ready?i.href:'#'}" class="nav-item ${a} ${d}"><span class="nav-item-icon">${i.icon}</span><span class="nav-item-label">${i.label}</span>${b}</a>`;
  }).join('');
  return`<a href="index.html" class="sidebar-logo"><span class="sidebar-logo-icon">⛏️</span><div><div class="sidebar-logo-text">MineGuide</div><div class="sidebar-logo-sub">Java 1.16 – 1.21.4</div></div></a>
    <nav class="sidebar-nav"><div class="sidebar-section">Outils</div>${items}<div class="sidebar-section" style="margin-top:6px">Navigation</div>
    <a href="index.html" class="nav-item ${cur==='index'?'active':''}"><span class="nav-item-icon">🏠</span><span class="nav-item-label">Accueil</span></a></nav>
    <div class="sidebar-auth"><div id="auth-widget-container"></div></div>
    <div class="sidebar-version"><div class="version-dot"></div>Java 1.16 – 1.21.4</div>`;
}
function buildFooter(){
  const ready=NAV_ITEMS.filter(i=>i.ready);
  const links=ready.map(i=>`<a href="${i.href}" class="footer-col-link" style="font-size:10px;color:var(--border2);text-decoration:none;">${i.label}</a>`).join('<span class="footer-sep">·</span>');
  return`<div class="footer-bottom">
    <span class="footer-logo-text">⛏️ MineGuide</span>
    <span class="footer-sep">—</span>
    ${links}
    <span class="footer-spacer"></span>
    <span>Non affilié à Mojang</span>
    <span class="footer-sep">·</span>
    <span>© 2025 MineGuide</span>
  </div>`;
}
function renderAuthWidget(user){
  const c=document.getElementById('auth-widget-container');if(!c)return;
  if(user){
    const init=(user.displayName||user.email||'?')[0].toUpperCase();
    const name=user.displayName||(user.email?user.email.split('@')[0]:'Joueur');
    c.innerHTML=`<a href="profile.html" class="auth-widget"><div class="auth-avatar">${user.photoURL?`<img src="${user.photoURL}" alt="">`:init}</div><div class="auth-info"><div class="auth-name">${name}</div><div class="auth-status">Mon profil</div></div><span class="auth-arrow">›</span></a>`;
  }else{
    c.innerHTML=`<a href="login.html" class="auth-widget"><div class="auth-avatar" style="background:var(--bg4);border-color:var(--border2);color:var(--gray);font-size:15px">👤</div><div class="auth-info"><div class="auth-name" style="color:var(--gray)">Se connecter</div><div class="auth-status">Sauvegarder vos seeds</div></div><span class="auth-arrow">›</span></a>`;
  }
}
function showToast(msg,type='info',dur=3000){
  const icons={success:'✅',error:'❌',info:'ℹ️'};
  const c=document.getElementById('toast-container');if(!c)return;
  const el=document.createElement('div');el.className=`toast ${type}`;
  el.innerHTML=`<span>${icons[type]||'•'}</span><span>${msg}</span>`;c.appendChild(el);
  setTimeout(()=>{el.style.transition='opacity .3s';el.style.opacity='0';setTimeout(()=>el.remove(),300);},dur);
}
document.addEventListener('DOMContentLoaded',()=>{
  const sb=document.getElementById('sidebar');if(sb)sb.innerHTML=buildSidebar();
  const ft=document.getElementById('footer');if(ft)ft.innerHTML=buildFooter();
  if(!document.getElementById('toast-container')){const tc=document.createElement('div');tc.id='toast-container';document.body.appendChild(tc);}
  renderAuthWidget(null);
});
window.MineGuide={showToast,renderAuthWidget,getCurrentPage};
