from fastapi import APIRouter, HTTPException
from pydantic import BaseModel

router = APIRouter(
    prefix="/auth",
    tags=["Authentication"]
)


class LoginRequest(BaseModel):
    username: str
    password: str


@router.post("/login")
async def login(credentials: LoginRequest):

    #
    # Temporary hardcoded authentication.
    #
    # This will later become a PostgreSQL query.
    #

    if (
        credentials.username == "admin"
        and
        credentials.password == "admin"
    ):

        return {
            "success": True,
            "username": "admin",
            "role": "administrator"
        }

    raise HTTPException(
        status_code=401,
        detail="Invalid username or password."
    )