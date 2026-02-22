/* ============================================================
   MINEGUIDE — auth.js
   Gestion de l'authentification Firebase
   ============================================================

   CONFIGURATION :
   1. Créez un projet sur https://console.firebase.google.com
   2. Activez Authentication > Email/Password et Google
   3. Copiez votre firebaseConfig ci-dessous
   ============================================================ */

// ⚠️ Remplacez par votre configuration Firebase
const FIREBASE_CONFIG = {
  apiKey:            "VOTRE_API_KEY",
  authDomain:        "VOTRE_PROJECT.firebaseapp.com",
  projectId:         "VOTRE_PROJECT_ID",
  storageBucket:     "VOTRE_PROJECT.appspot.com",
  messagingSenderId: "VOTRE_SENDER_ID",
  appId:             "VOTRE_APP_ID",
};

// ============================================================
// MODE DÉMO (si Firebase non configuré)
// ============================================================
const IS_DEMO = FIREBASE_CONFIG.apiKey === "VOTRE_API_KEY";

if (IS_DEMO) {
  console.warn('[MineGuide] Firebase non configuré — mode démo actif.');
}

// ============================================================
// INTERFACE AUTH (compatible Firebase ou mode démo)
// ============================================================

let _currentUser = null;
let _authListeners = [];

// Lit l'utilisateur depuis localStorage (mode démo)
function _loadDemoUser() {
  try {
    const saved = localStorage.getItem('mg_demo_user');
    return saved ? JSON.parse(saved) : null;
  } catch { return null; }
}

function _saveDemoUser(user) {
  if (user) localStorage.setItem('mg_demo_user', JSON.stringify(user));
  else localStorage.removeItem('mg_demo_user');
}

function _notifyListeners(user) {
  _currentUser = user;
  _authListeners.forEach(fn => fn(user));
  if (typeof window.MineGuide !== 'undefined') {
    window.MineGuide.renderAuthWidget(user);
  }
}

/* --- Connexion email/mot de passe --- */
async function signInWithEmail(email, password) {
  if (IS_DEMO) {
    // Simulation
    await _delay(600);
    const users = _getDemoUsers();
    const user = users.find(u => u.email === email);
    if (!user) throw new Error('Aucun compte trouvé avec cet e-mail.');
    if (user.password !== password) throw new Error('Mot de passe incorrect.');
    const authUser = { uid: user.uid, email: user.email, displayName: user.displayName, photoURL: null };
    _saveDemoUser(authUser);
    _notifyListeners(authUser);
    return authUser;
  }
  // Firebase réel
  const { signInWithEmailAndPassword } = await _getFirebaseAuth();
  const cred = await signInWithEmailAndPassword(_auth, email, password);
  return cred.user;
}

/* --- Inscription email/mot de passe --- */
async function createAccount(email, password, displayName) {
  if (IS_DEMO) {
    await _delay(600);
    const users = _getDemoUsers();
    if (users.find(u => u.email === email)) throw new Error('Un compte existe déjà avec cet e-mail.');
    const newUser = {
      uid: 'demo_' + Date.now(),
      email,
      password,
      displayName: displayName || email.split('@')[0],
    };
    users.push(newUser);
    _saveDemoUsers(users);
    const authUser = { uid: newUser.uid, email: newUser.email, displayName: newUser.displayName, photoURL: null };
    _saveDemoUser(authUser);
    _notifyListeners(authUser);
    return authUser;
  }
  const { createUserWithEmailAndPassword, updateProfile } = await _getFirebaseAuth();
  const cred = await createUserWithEmailAndPassword(_auth, email, password);
  if (displayName) await updateProfile(cred.user, { displayName });
  _notifyListeners(cred.user);
  return cred.user;
}

/* --- Connexion Google --- */
async function signInWithGoogle() {
  if (IS_DEMO) {
    await _delay(400);
    const authUser = {
      uid: 'demo_google_1',
      email: 'joueur@gmail.com',
      displayName: 'Joueur Demo',
      photoURL: null,
    };
    _saveDemoUser(authUser);
    _notifyListeners(authUser);
    return authUser;
  }
  const { GoogleAuthProvider, signInWithPopup } = await _getFirebaseAuth();
  const provider = new GoogleAuthProvider();
  const cred = await signInWithPopup(_auth, provider);
  return cred.user;
}

/* --- Déconnexion --- */
async function signOut() {
  if (IS_DEMO) {
    _saveDemoUser(null);
    _notifyListeners(null);
    return;
  }
  const { signOut: fbSignOut } = await _getFirebaseAuth();
  await fbSignOut(_auth);
}

/* --- Réinitialisation mot de passe --- */
async function resetPassword(email) {
  if (IS_DEMO) {
    await _delay(500);
    return; // Simulé
  }
  const { sendPasswordResetEmail } = await _getFirebaseAuth();
  await sendPasswordResetEmail(_auth, email);
}

/* --- Écouter les changements d'auth --- */
function onAuthChange(callback) {
  _authListeners.push(callback);
  // Appel immédiat avec l'état actuel
  callback(_currentUser);
  return () => {
    _authListeners = _authListeners.filter(fn => fn !== callback);
  };
}

/* --- Obtenir l'utilisateur actuel --- */
function getCurrentUser() { return _currentUser; }

/* --- Rediriger si non connecté --- */
function requireAuth(redirectTo = 'login.html') {
  if (!_currentUser) {
    window.location.href = redirectTo + '?redirect=' + encodeURIComponent(window.location.href);
  }
}

// ============================================================
// FIRESTORE (sauvegarde de données utilisateur)
// ============================================================

async function saveUserData(key, data) {
  const user = _currentUser;
  if (!user) throw new Error('Non connecté.');
  if (IS_DEMO) {
    const store = _getDemoStore(user.uid);
    store[key] = data;
    _saveDemoStore(user.uid, store);
    return;
  }
  // Firebase Firestore
  const { doc, setDoc, getFirestore } = await _getFirestore();
  await setDoc(doc(getFirestore(), 'users', user.uid, 'data', key), data, { merge: true });
}

async function loadUserData(key) {
  const user = _currentUser;
  if (!user) return null;
  if (IS_DEMO) {
    const store = _getDemoStore(user.uid);
    return store[key] || null;
  }
  const { doc, getDoc, getFirestore } = await _getFirestore();
  const snap = await getDoc(doc(getFirestore(), 'users', user.uid, 'data', key));
  return snap.exists() ? snap.data() : null;
}

// ============================================================
// HELPERS INTERNES
// ============================================================
let _auth = null;
let _firebaseApp = null;

async function _getFirebaseAuth() {
  if (!_firebaseApp) {
    const { initializeApp } = await import('https://www.gstatic.com/firebasejs/10.7.1/firebase-app.js');
    _firebaseApp = initializeApp(FIREBASE_CONFIG);
  }
  const module = await import('https://www.gstatic.com/firebasejs/10.7.1/firebase-auth.js');
  if (!_auth) _auth = module.getAuth(_firebaseApp);
  return module;
}

async function _getFirestore() {
  if (!_firebaseApp) await _getFirebaseAuth();
  return import('https://www.gstatic.com/firebasejs/10.7.1/firebase-firestore.js');
}

function _delay(ms) { return new Promise(r => setTimeout(r, ms)); }
function _getDemoUsers() { try { return JSON.parse(localStorage.getItem('mg_demo_users') || '[]'); } catch { return []; } }
function _saveDemoUsers(u) { localStorage.setItem('mg_demo_users', JSON.stringify(u)); }
function _getDemoStore(uid) { try { return JSON.parse(localStorage.getItem(`mg_store_${uid}`) || '{}'); } catch { return {}; } }
function _saveDemoStore(uid, data) { localStorage.setItem(`mg_store_${uid}`, JSON.stringify(data)); }

// ============================================================
// INIT — écoute les changements Firebase ou charge depuis localStorage
// ============================================================
async function _initAuth() {
  if (IS_DEMO) {
    const user = _loadDemoUser();
    _notifyListeners(user);
    return;
  }
  try {
    const { onAuthStateChanged } = await _getFirebaseAuth();
    onAuthStateChanged(_auth, user => _notifyListeners(user || null));
  } catch (e) {
    console.error('[MineGuide] Erreur Firebase Auth:', e);
    _notifyListeners(null);
  }
}

_initAuth();

// Export global
window.Auth = {
  signInWithEmail,
  createAccount,
  signInWithGoogle,
  signOut,
  resetPassword,
  onAuthChange,
  getCurrentUser,
  requireAuth,
  saveUserData,
  loadUserData,
  IS_DEMO,
};
