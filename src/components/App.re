[@bs.module "../lib/init-uppy.js"] external initUppy: unit => unit = "default";

let component = ReasonReact.statelessComponent("App");

type data = {. "site": {. "siteMetadata": {. "title": string}}};

let meta = [|
  Helmet.metaField(~name="description", ~content="Sample"),
  Helmet.metaField(~name="keywords", ~content="sample, something"),
|];

let make = (~data, _children) => {
  ...component,
  render: _self =>
    <div>
      <Helmet title=data##site##siteMetadata##title meta>
        <link
          href="https://transloadit.edgly.net/releases/uppy/v0.27.5/dist/uppy.min.css"
          rel="stylesheet"
        />
      </Helmet>
      <Header siteTitle=data##site##siteMetadata##title />
      <BrowserWeb3Capabilities isLoggedIn=true loggedInAddress="123456789">
        ...{
             ({hasWeb3}) =>
               <p>
                 "Have you seen a component HMR this quick"->ReasonReact.string
                 <br />
                 ("OCaml ftw " ++ hasWeb3->string_of_bool)->ReasonReact.string
               </p>
           }
      </BrowserWeb3Capabilities>
      <button onClick={_ => initUppy()} id="select-files">
        "CLICK ME"->ReasonReact.string
      </button>
      <GatsbyLink
        style={ReactDOMRe.Style.make(~margin="0", ())} to_="/page-2">
        {ReasonReact.string("GatsbyLink To Page 2")}
      </GatsbyLink>
    </div>,
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps =>
    make(~data=jsProps##data, jsProps##children)
  );