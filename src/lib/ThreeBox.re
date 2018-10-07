type threeBox;
type web3Provider;
type web3 = {
  .
  "eth": {. "accounts": array(string)},
  "currentProvider": web3Provider,
};
[@bs.val] external web3: web3 = "window.web3";
[@bs.module "3box"]
external openBox: (string, web3Provider) => Js.Promise.t(threeBox) = "";

[@bs.send] external logout: threeBox => Js.Promise.t(unit) = "logout";