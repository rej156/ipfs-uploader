type web3 = {. "eth": {. "accounts": array(string)}};
[@bs.val] external web3: Js.Nullable.t(web3) = "window.web3";

type state = {
  hasWeb3: bool,
  isLockedAccount: bool,
  isCurrentWeb3AddressSameAsLoggedInAddress: bool,
  currentNetworkName: string,
  isLoggedIn: bool,
};

type action =
  | SetLockedAccountStatus(bool)
  | SetCurrentWeb3AddressSameAsLoggedInAddress(bool)
  | SetCurrentNetworkName(string)
  | SetWeb3Status(bool);
let component = ReasonReact.reducerComponent(__MODULE__);

let getWeb3BoundStatus = conditional =>
  Js.Nullable.toOption(web3)->Belt.Option.mapWithDefault(false, conditional);

let make = (~isLoggedIn, ~loggedInAddress, children) => {
  ...component,
  initialState: () => {
    hasWeb3: false,
    isLockedAccount: false,
    isCurrentWeb3AddressSameAsLoggedInAddress: false,
    currentNetworkName: "",
    isLoggedIn,
  },
  didMount: self => {
    let getWeb3StatusIntervalId =
      Js.Global.setInterval(
        () => self.send(SetWeb3Status(getWeb3BoundStatus(_ => true))),
        1000,
      );
    let getLockedAccountStatusIntervalId =
      Js.Global.setInterval(
        () =>
          self.send(
            SetLockedAccountStatus(
              getWeb3BoundStatus(web3 =>
                Js.Array.length(web3##eth##accounts) === 0
              ),
            ),
          ),
        1000,
      );

    let getCurrentWeb3AddressSameAsLoggedInAccountStatusIntervalId =
      Js.Global.setInterval(
        () =>
          self.send(
            SetCurrentWeb3AddressSameAsLoggedInAddress(
              getWeb3BoundStatus(web3 =>
                isLoggedIn && Js.Array.length(web3##eth##accounts) > 0 ?
                  web3##eth##accounts[0] === loggedInAddress : false
              ),
            ),
          ),
        1000,
      );

    self.onUnmount(() => {
      Js.Global.clearInterval(getWeb3StatusIntervalId);
      Js.Global.clearInterval(getLockedAccountStatusIntervalId);
      Js.Global.clearInterval(
        getCurrentWeb3AddressSameAsLoggedInAccountStatusIntervalId,
      );
    });
  },
  reducer: (action, state) =>
    switch (action) {
    | SetWeb3Status(status) =>
      ReasonReact.Update({...state, hasWeb3: status})
    | SetLockedAccountStatus(status) =>
      ReasonReact.Update({...state, isLockedAccount: status})
    | SetCurrentWeb3AddressSameAsLoggedInAddress(status) =>
      ReasonReact.Update({
        ...state,
        isCurrentWeb3AddressSameAsLoggedInAddress: status,
      })
    | SetCurrentNetworkName(name) =>
      ReasonReact.Update({...state, currentNetworkName: name})
    },
  render: self => children(self.state),
};