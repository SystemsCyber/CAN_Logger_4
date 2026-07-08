from io import BytesIO
from minio import Minio
from minio.error import S3Error


class Storage:
    def __init__(self):
        self.bucket = "rods"

        self.client = Minio(
            endpoint="minio:9000",
            access_key="minio",
            secret_key="miniopassword",
            secure=False
        )

        self.initialize()

    def initialize(self):
        """
        Ensure the bucket exists.
        """
        if not self.client.bucket_exists(self.bucket):
            self.client.make_bucket(self.bucket)
            print(f"[Storage] Created bucket '{self.bucket}'")
        else:
            print(f"[Storage] Bucket '{self.bucket}' already exists")

    def upload(self, filename: str, data: bytes):
        """
        Upload a file to MinIO.
        """

        self.client.put_object(
            bucket_name=self.bucket,
            object_name=filename,
            data=BytesIO(data),
            length=len(data),
            content_type="application/octet-stream"
        )

        return filename

    def download(self, filename: str):
        """
        Download a file from MinIO.
        """

        response = self.client.get_object(
            self.bucket,
            filename
        )

        return response.read()

    def delete(self, filename: str):
        """
        Delete a file from MinIO.
        """

        self.client.remove_object(
            self.bucket,
            filename
        )

    def list_files(self):

        files = []

        for obj in self.client.list_objects(
            self.bucket,
            recursive=True
        ):

            files.append({
                "name": obj.object_name,
                "size": obj.size,
                "modified": obj.last_modified
            })

        return files

storage = Storage()