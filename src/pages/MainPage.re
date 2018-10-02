let component = ReasonReact.statelessComponent("index");

let text = ReasonReact.string;

let make = _children => {
  ...component,
  render: _self =>
    <div>
      <BrowserWeb3Capabilities
        isLoggedIn=true
        loggedInAddress="0xbfecec47dd8bf5f6264a9830a9d26ef387c38a67">
        ...{
             (
               {
                 hasWeb3,
                 isLockedAccount,
                 isCurrentWeb3AddressSameAsLoggedInAddress,
                 currentNetworkName,
               },
             ) =>
               <div> <p> "has Web3"->ReasonReact.string </p> </div>
           }
      </BrowserWeb3Capabilities>
    </div>,
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps => make(jsProps##children));