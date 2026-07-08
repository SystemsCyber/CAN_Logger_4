from fastapi import APIRouter, UploadFile, File, HTTPException

from app.storage import storage

router = APIRouter()


@router.get("/health")
def health():
    return {
        "status": "healthy",
        "version": "0.1.0"
    }


@router.post("/upload")
async def upload(file: UploadFile = File(...)):
    try:
        data = await file.read()

        storage.upload(
            filename=file.filename,
            data=data
        )

        return {
            "success": True,
            "filename": file.filename,
            "size": len(data)
        }

    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=str(e)
        )

@router.get("/files")
def files():

    return storage.list_files()