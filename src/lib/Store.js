const prefixKey = key => 'ipfsUploader/' + key

export const storeFile = (box, ipfsHash) =>
  box.private.get(prefixKey('files')).then(files => {
    const newFiles = Array.isArray(files) ? files : []
    newFiles.push(ipfsHash)
    box.private
      .set(prefixKey('files'), newFiles)
      .then(result => console.log(result))
  })
