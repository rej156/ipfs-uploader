type web3 = {. "eth": {. "accounts": array(string)}};
[@bs.val] external web3: Js.Nullable.t(web3) = "window.web3";

type state = {
  hasWeb3: bool,
  isLockedAccount: bool,
};

/* Action declaration */
type action =
  | SetLockedAccountStatus(bool)
  | SetWeb3Status(bool);
let component = ReasonReact.reducerComponent(__MODULE__);

let getWeb3BoundStatus = conditional =>
  Js.Nullable.toOption(web3)->Belt.Option.mapWithDefault(false, conditional);

let make = children => {
  ...component,
  initialState: () => {hasWeb3: false, isLockedAccount: false},
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

    self.onUnmount(() => {
      Js.Global.clearInterval(getWeb3StatusIntervalId);
      Js.Global.clearInterval(getLockedAccountStatusIntervalId);
    });
  },
  reducer: (action, state) =>
    switch (action) {
    | SetWeb3Status(status) =>
      ReasonReact.Update({...state, hasWeb3: status})
    | SetLockedAccountStatus(status) =>
      ReasonReact.Update({...state, isLockedAccount: status})
    },
  render: self => children(self.state),
};