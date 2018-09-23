type web3 = {. "version": {. "api": string}};
[@bs.val] external web3: Js.Nullable.t(web3) = "window.web3";

type state = {
  hasWeb3: bool,
  isLoggedIn: bool,
};

/* Action declaration */
type action =
  | SetWeb3Status(bool);
let component = ReasonReact.reducerComponent(__MODULE__);

let make = children => {
  ...component,
  initialState: () => {hasWeb3: false, isLoggedIn: false},
  didMount: self => {
    let getWeb3StatusIntervalId =
      Js.Global.setInterval(
        () =>
          self.send(
            SetWeb3Status(
              Js.Nullable.toOption(web3)
              ->Belt.Option.mapWithDefault(false, _web3
                  /* Js.log(web3##version##api); */
                  => true),
            ),
          ),
        1000,
      );
    self.onUnmount(() => Js.Global.clearInterval(getWeb3StatusIntervalId));
  },
  reducer: (action, state) =>
    switch (action) {
    | SetWeb3Status(status) =>
      ReasonReact.Update({...state, hasWeb3: status})
    },
  render: self => children(self.state),
};