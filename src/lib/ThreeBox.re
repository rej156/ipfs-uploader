type threeBox;
type web3Provider;
type web3 = {
  .
  "eth": {. "accounts": array(string)},
  "currentProvider": web3Provider,
};
[@bs.val] external web3: web3 = "window.web3";
[@bs.val "window.ThreeBox.openBox"]
external openBox: (string, web3Provider) => Promise.t(threeBox) = "";

[@bs.send] external logout: threeBox => unit = "logout";